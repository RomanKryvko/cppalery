#include "previewWindow.h"
#include "window.h"

PreviewWindow::PreviewWindow() = default;

PreviewWindow::PreviewWindow(int height, int width, int startY, int startX) : Window(height, width) {
    this->window = newwin(height, width, startY, startX);
}

PreviewWindow::~PreviewWindow() {
    if (isUeberzugRunning) {
        terminateImgPreview();
    }
}

void PreviewWindow::resetSetup() {
    werase(window);
    wrefresh(window);
}

void PreviewWindow::move(int newHeight, int newWidth, int startX, int startY) {
    // for some reason I coudn't get the mvwin to work
    delwin(window);
    window = newwin(newHeight, newWidth, startX, startY);
    resize(newHeight, newWidth);
}

void PreviewWindow::resize(int newHeight, int newWidth) {
    this->width = newWidth;
    this->height = newHeight;
}

void PreviewWindow::startUeberzug() {
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
        isUeberzugRunning = true;
    }
}

void PreviewWindow::renderImg(const std::string &path, int height, int width, int startX, int startY) {
    std::ostringstream oss;
    oss << "{\"action\": \"add\", \"synchronously_draw\": true, \"identifier\": \"preview\", \"x\": " << startX << ", \"y\": " << startY << ", \"scaler\": \"contain\", \"width\": " << width << ", \"height\": " << height << ", \"path\": \"" << path << "\"}";
    std::string command = oss.str();

    if (!isUeberzugRunning) {
        startUeberzug();
    }
    write(stdin_pipe[1], command.c_str(), command.size());
    write(stdin_pipe[1], "\n", 1);
}

void PreviewWindow::terminateImgPreview() {
    close(stdin_pipe[1]);
    int status;
    waitpid(pid, &status, 0);
    isUeberzugRunning = false;
}
