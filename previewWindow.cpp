#include "cppalery.h"
#include "directory.h"
#include "window.h"
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

class PreviewWindow : public Window {
    private:
        pid_t pid = -1;
        int stdin_pipe[2];
        int stdout_pipe[2];

    public:
        bool isUeberzugRunning = false;

        PreviewWindow() {}
        PreviewWindow(int height, int width, int startY, int startX) {
            this->height = height;
            this->width = width;
            this->window = newwin(this->height, this->width, startY, startX);
        }

        void initialize(int height, int width, int startY, int startX) {
            this->height = height;
            this->width = width;
            this->window = newwin(this->height, this->width, startY, startX);
        }

        virtual void resetSetup() {
            werase(this->window);
            box(this->window, 0, 0);
            wrefresh(this->window);
        }

        void move(int newHeigth, int newWidth, int startX, int startY) {
            // for some reason I coudn't get the mvwin to work
            delwin(this->window);
            this->window = newwin(newHeigth, newWidth, startX, startY);
        }

        void startUeberzug() {
            if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                // Redirect stdin and stdout
                dup2(stdin_pipe[0], STDIN_FILENO);
                dup2(stdout_pipe[1], STDOUT_FILENO);
                close(stdin_pipe[1]);
                close(stdout_pipe[0]);

                // Close stderr to /dev/null
                int devnull = open("/dev/null", O_WRONLY);
                if (devnull == -1) {
                    perror("open /dev/null");
                    exit(EXIT_FAILURE);
                }
                dup2(devnull, STDERR_FILENO);
                close(devnull);

                execlp("ueberzug", "ueberzug", "layer", "--silent", NULL);
                perror("execlp");
                exit(EXIT_FAILURE);
            } else {
                close(stdin_pipe[0]);
                close(stdout_pipe[1]);
                this->isUeberzugRunning = true;
            }
        }

        void renderImg(const std::string &path, int height, int width, int startX, int startY) {
            std::ostringstream oss;
            oss << "{\"action\": \"add\", \"synchronously_draw\": true, \"identifier\": \"preview\", \"x\": " << startX << ", \"y\": " << startY << ", \"scaler\": \"contain\", \"width\": " << width << ", \"height\": " << height << ", \"path\": \"" << path << "\"}";
            std::string command = oss.str();

            if (!isUeberzugRunning) {
                this->startUeberzug();
            }
            write(stdin_pipe[1], command.c_str(), command.size());
            write(stdin_pipe[1], "\n", 1);
        }

        void terminateImgPreview() {
            close(stdin_pipe[1]);
            int status;
            waitpid(pid, &status, 0);
            this->isUeberzugRunning = false;
        }
};