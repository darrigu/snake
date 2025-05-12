#include "./back.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define TRUE 1
#define FALSE 0

#define ASSERT(cond, message) do { if (!(cond)) front_panic(__FILE__, __LINE__, message); } while (0)
#define UNREACHABLE() front_panic(__FILE__, __LINE__, "unreachable")

#define CELL_SIZE FACTOR
#define COLS (WIDTH/CELL_SIZE)
#define ROWS (HEIGHT/CELL_SIZE)

#define BACKGROUND_COLOR 0xFFA1E3A6
#define CELL1_COLOR BACKGROUND_COLOR
#define CELL2_COLOR 0xFF79D781
#define SNAKE_BODY_COLOR 0xFF2E2E1E
#define APPLE_COLOR 0xFFA88BF3

#define STEP_INTERVAL 0.1f

#define RAND_A 6364136223846793005
#define RAND_C 1442695040888963407

static u32 rand(void) {
  static u64 rand_state = 0;
  rand_state = rand_state*RAND_A + RAND_C;
  return (rand_state >> 32)&0xFF;
}

typedef enum {
  DIR_RIGHT,
  DIR_UP,
  DIR_LEFT,
  DIR_DOWN,
  COUNT_DIRS,
} Dir;

Dir dir_opposite(Dir dir) {
  switch (dir) {
  case DIR_RIGHT: return DIR_LEFT;
  case DIR_LEFT:  return DIR_RIGHT;
  case DIR_UP:    return DIR_DOWN;
  case DIR_DOWN:  return DIR_UP;
  case COUNT_DIRS:
  default: UNREACHABLE();
  }
  return 0;
}

typedef struct {
  s32 x, y;
} Cell;

#define SNAKE_CAP (ROWS*COLS)
typedef struct {
  Cell body[SNAKE_CAP];
  u32 begin;
  u32 size;
} Snake;

typedef struct {
  Snake snake;
  Cell apple;
  Dir dir;
  Dir next_dir;
  f32 step_cooldown;
  s32 one_time;
} Game;

static Game game = {
  .snake = {
    .size = 3,
    .body = {
      [0] = {.x = 0, .y = ROWS/2},
      [1] = {.x = 1, .y = ROWS/2},
      [2] = {.x = 2, .y = ROWS/2},
    }
  },
  .dir = DIR_RIGHT,
};

int game_width(void) {
  return WIDTH;
}

int game_height(void) {
  return HEIGHT;
}

static void snake_render(Snake* snake) {
  for (u32 offset = 0; offset < snake->size; ++offset) {
    u32 index = (snake->begin + offset)%SNAKE_CAP;
    Cell* cell = &snake->body[index];
    front_fill_rect(cell->x*CELL_SIZE, cell->y*CELL_SIZE, CELL_SIZE, CELL_SIZE, SNAKE_BODY_COLOR);
  }
}

static void background_render(void) {
  front_fill_rect(0, 0, WIDTH, HEIGHT, BACKGROUND_COLOR);
  for (s32 col = 0; col < COLS; ++col) {
    for (s32 row = 0; row < ROWS; ++row) {
      u32 color = (row + col)%2 == 0 ? CELL1_COLOR : CELL2_COLOR;
      s32 x = col*CELL_SIZE;
      s32 y = row*CELL_SIZE;
      s32 w = CELL_SIZE;
      s32 h = CELL_SIZE;
      front_fill_rect(x, y, w, h, color);
    }
  }
}

Cell* snake_head(Snake* snake) {
  ASSERT(snake->size > 0, "snake is empty");
  u32 index = (snake->begin + snake->size - 1)%SNAKE_CAP;
  return &snake->body[index];
}

s32 emod(s32 a, s32 b) {
  return (a%b + b)%b;
}

Cell step_cell(Cell head, Dir dir) {
  switch (dir) {
  case DIR_RIGHT:
    head.x += 1;
    break;
  case DIR_UP:
    head.y -= 1;
    break;
  case DIR_LEFT:
    head.x -= 1;
    break;
  case DIR_DOWN:
    head.y += 1;
    break;
  case COUNT_DIRS:
  default: UNREACHABLE();
  }

  head.x = emod(head.x, COLS);
  head.y = emod(head.y, ROWS);
  return head;
}

void snake_push_head(Snake* snake, Cell head) {
  ASSERT(snake->size < SNAKE_CAP, "snake overflow");
  u32 index = (snake->begin + snake->size)%SNAKE_CAP;
  snake->body[index] = head;
  snake->size += 1;
}

void snake_pop_tail(Snake* snake) {
  ASSERT(snake->size > 0, "snake underflow");
  snake->begin = (snake->begin + 1)%SNAKE_CAP;
  snake->size -= 1;
}

void game_render(void) {
  background_render();
  front_fill_rect(game.apple.x*CELL_SIZE, game.apple.y*CELL_SIZE, CELL_SIZE, CELL_SIZE, APPLE_COLOR);
  snake_render(&game.snake);
}

b32 cell_eq(const Cell* a, const Cell* b) {
  return a->x == b->x && a->y == b->y;
}

b32 is_cell_snake_body(const Cell* cell) {
  for (u32 offset = 0; offset < game.snake.size; offset++) {
    u32 index = (game.snake.begin + offset)%SNAKE_CAP;
    if (cell_eq(&game.snake.body[index], cell)) {
      return TRUE;
    }
  }
  return FALSE;
}

void random_apple(void) {
  do {
    game.apple.x = rand()%COLS;
    game.apple.y = rand()%ROWS;
  } while (is_cell_snake_body(&game.apple));
}

void game_update(f32 dt) {
  game.step_cooldown -= dt;
  if (game.step_cooldown <= 0.0f) {
    if (dir_opposite(game.dir) != game.next_dir) {
      game.dir = game.next_dir;
    }

    Cell* head = snake_head(&game.snake);
    Cell next_head = step_cell(*head, game.dir);
    snake_push_head(&game.snake, next_head);

    if (cell_eq(&game.apple, head)) {
      random_apple();
    } else {
      snake_pop_tail(&game.snake);
    }
    
    game.step_cooldown = STEP_INTERVAL;
  }

  if (front_keydown(KEY_UP))    game.next_dir = DIR_UP;
  if (front_keydown(KEY_LEFT))  game.next_dir = DIR_LEFT;
  if (front_keydown(KEY_DOWN))  game.next_dir = DIR_DOWN;
  if (front_keydown(KEY_RIGHT)) game.next_dir = DIR_RIGHT;
}
