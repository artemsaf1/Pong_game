#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 25
#define MAX_SCORE 21

// Глобальные переменные для позиций
int score1;
int score2;
char player1[20];
char player2[20];
int ball_x = WIDTH / 3, ball_y = HEIGHT / 3;
int ball_dx = 1, ball_dy = 1;
int left_paddle = 1;
int right_paddle = 1;
int score_left = 0, score_right = 0;  // Счёт игроков

// Функция настройки терминала для небуферизованного ввода
void enable_raw_mode() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Функция проверки ввода без ожидания
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Отображение игрового поля + счёт
void draw() {
    system("clear");
    printf("Игрок 1 %s: %d    Игрок 2 %s: %d\n", player1, score_left, player2, score_right);  // Вывод счёта
    printf("--------------------------------------------------------------------------------\n");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 && (y == left_paddle || y == left_paddle + 1 || y == left_paddle - 1)) {
                printf("|");  // Левая ракетка
            } else if (x == WIDTH - 1 && (y == right_paddle ||
             y == right_paddle + 1 || y == right_paddle - 1)) {
                printf("|");  // Правая ракетка
            } else if (x == ball_x && y == ball_y) {
                printf("O");  // Мяч
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("--------------------------------------------------------------------------------\n");
}

// Сброс позиции мяча после гола
void reset_ball() {
    unsigned int seed = 42;
    ball_x = WIDTH / 2;
    ball_y = HEIGHT / 2;
    ball_dx = (ball_dx > 0) ? -1 : 1;  // Меняем направление
    ball_dy = (rand_r(&seed) % 2 == 0) ? 1 : -1;  // Случайное отклонение
}

// Обновление позиции мяча
void update_ball() {
    ball_x += ball_dx;
    ball_y += ball_dy;
    // Отскок от верхней и нижней границы
    if (ball_y == 0 || ball_y == HEIGHT - 1) {
        ball_dy = -ball_dy;
    }

    // Проверка на отскок от ракеток
    if (ball_x == 1 && (ball_y == left_paddle || ball_y == left_paddle + 1)) {
        ball_dx = -ball_dx;
    }
    if (ball_x == WIDTH - 2 && (ball_y == right_paddle || ball_y == right_paddle + 1)) {
        ball_dx = -ball_dx;
    }

    // Проверка на гол
    if (ball_x <= 0) {
        score_right++;  // Гол в левые ворота
        score2++;
        reset_ball();
    }
    if (ball_x >= WIDTH - 1) {
        score_left++;  // Гол в правые ворота
        score1++;
        reset_ball();
    }
}

// Обновление управления
void update_paddles() {
    if (kbhit()) {
        char key = getchar();
        if (key == 'a' && left_paddle > 1) left_paddle--;
        if (key == 'z' && left_paddle < 23) left_paddle++;
        if (key == 'k' && right_paddle > 1) right_paddle--;
        if (key == 'm' && right_paddle < 23) right_paddle++;
    }
}

// Основной игровой цикл
int main() {
    printf("Введите имя Игрока 1:\n");
    scanf("%s", player1);
    printf("Введите имя Игрока 2:\n");
    scanf("%s", player2);
    enable_raw_mode();
    while (1) {
        draw();
        update_paddles();
        update_ball();

        if (score_left >= MAX_SCORE || score_right >= MAX_SCORE) {
            system("clear");
            printf("Game Over!\n");
            if (score_left >= MAX_SCORE) {
            printf("Игрок 1 %s побеждает со счетом %d:%d!\n", player1, score1, score2);
        } else {
            printf("Игрок 2 %s побеждает со счетом %d:%d!\n", player2, score1, score2);
        }
        break;
    }
    usleep(100000);
    }
    return 0;
}
