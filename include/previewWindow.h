#ifndef PREVIEW_WINDOW_H
#define PREVIEW_WINDOW_H

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

        PreviewWindow();
        PreviewWindow(int height, int width, int startY, int startX);
        ~PreviewWindow();

        void initialize(int height, int width, int startY, int startX);

        void resetSetup() override;

        void move(int newHeigth, int newWidth, int startX, int startY);

        void startUeberzug();

        void renderImg(const std::string &path, int height, int width, int startX, int startY);

        void terminateImgPreview();
};

#endif