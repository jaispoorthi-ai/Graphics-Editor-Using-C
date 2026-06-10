#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ROWS        25
#define COLS        60
#define MAX_OBJECTS 50
#define BLANK       '_'
#define DRAW        '*'
#define SHAPE_CIRCLE    1
#define SHAPE_RECT      2
#define SHAPE_LINE      3
#define SHAPE_TRIANGLE  4
typedef struct {
    int type;  
int x1, y1, x2, y2, x3, y3;
    int cx, cy, r;
    int active;        
} Shape;
char    canvas[ROWS][COLS];
Shape   objects[MAX_OBJECTS];
int     obj_count = 0;
void init_canvas(void)
{
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            canvas[r][c] = BLANK;
}

void set_pixel(int row, int col, char ch)
{
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
        canvas[row][col] = ch;
}

void display_canvas(void)
{
    printf("\n");
    /* Column ruler */
    printf("   ");
    for (int c = 0; c < COLS; c++)
        printf("%c", (c % 10 == 0) ? ('0' + (c / 10) % 10) : ' ');
    printf("\n   ");
    for (int c = 0; c < COLS; c++)
        printf("%d", c % 10);
    printf("\n");

    for (int r = 0; r < ROWS; r++) {
        printf("%2d ", r);
        for (int c = 0; c < COLS; c++)
            printf("%c", canvas[r][c]);
        printf("\n");
    }
    printf("\n");
}

void redraw_canvas(void);   
void draw_circle_on_canvas(int cx, int cy, int r, char ch)
{
    int x = 0, y = r;
    int d = 1 - r;

    while (x <= y) {
        set_pixel(cy + y, cx + x, ch);
        set_pixel(cy + y, cx - x, ch);
        set_pixel(cy - y, cx + x, ch);
        set_pixel(cy - y, cx - x, ch);
        set_pixel(cy + x, cx + y, ch);
        set_pixel(cy + x, cx - y, ch);
        set_pixel(cy - x, cx + y, ch);
        set_pixel(cy - x, cx - y, ch);

        if (d < 0)
            d += 2 * x + 3;
        else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}
void draw_line_on_canvas(int x1, int y1, int x2, int y2, char ch)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        set_pixel(y1, x1, ch);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}
void draw_rect_on_canvas(int x1, int y1, int x2, int y2, char ch)
{
   
    if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }

    for (int c = x1; c <= x2; c++) {
        set_pixel(y1, c, ch);
        set_pixel(y2, c, ch);
    }
    for (int r = y1; r <= y2; r++) {
        set_pixel(r, x1, ch);
        set_pixel(r, x2, ch);
    }
}
void draw_triangle_on_canvas(int x1,int y1,int x2,int y2,int x3,int y3, char ch)
{
    draw_line_on_canvas(x1, y1, x2, y2, ch);
    draw_line_on_canvas(x2, y2, x3, y3, ch);
    draw_line_on_canvas(x3, y3, x1, y1, ch);
}
void draw_shape(Shape *s, char ch)
{
    switch (s->type) {
        case SHAPE_CIRCLE:
            draw_circle_on_canvas(s->cx, s->cy, s->r, ch);
            break;
        case SHAPE_RECT:
            draw_rect_on_canvas(s->x1, s->y1, s->x2, s->y2, ch);
            break;
        case SHAPE_LINE:
            draw_line_on_canvas(s->x1, s->y1, s->x2, s->y2, ch);
            break;
        case SHAPE_TRIANGLE:
            draw_triangle_on_canvas(s->x1,s->y1,s->x2,s->y2,s->x3,s->y3, ch);
            break;
    }
}
void redraw_canvas(void)
{
    init_canvas();
    for (int i = 0; i < obj_count; i++)
        if (objects[i].active)
            draw_shape(&objects[i], DRAW);
}
const char *shape_name(int type)
{
    switch (type) {
        case SHAPE_CIRCLE:   return "Circle";
        case SHAPE_RECT:     return "Rectangle";
        case SHAPE_LINE:     return "Line";
        case SHAPE_TRIANGLE: return "Triangle";
        default:             return "Unknown";
    }
}

void list_objects(void)
{
    int found = 0;
    printf("\n=== Active Objects ===\n");
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        found = 1;
        printf("  [%d] %s  ", i, shape_name(objects[i].type));
        switch (objects[i].type) {
            case SHAPE_CIRCLE:
                printf("centre=(%d,%d) radius=%d",
                       objects[i].cx, objects[i].cy, objects[i].r);
                break;
            case SHAPE_RECT:
                printf("(%d,%d) -> (%d,%d)",
                       objects[i].x1, objects[i].y1,
                       objects[i].x2, objects[i].y2);
                break;
            case SHAPE_LINE:
                printf("(%d,%d) -> (%d,%d)",
                       objects[i].x1, objects[i].y1,
                       objects[i].x2, objects[i].y2);
                break;
            case SHAPE_TRIANGLE:
                printf("(%d,%d) (%d,%d) (%d,%d)",
                       objects[i].x1, objects[i].y1,
                       objects[i].x2, objects[i].y2,
                       objects[i].x3, objects[i].y3);
                break;
        }
        printf("\n");
    }
    if (!found) printf("  (no objects)\n");
    printf("\n");
}
int read_int(const char *prompt)
{
    int v;
    printf("%s", prompt);
    while (scanf("%d", &v) != 1) {
        printf("Invalid – enter an integer: ");
        while (getchar() != '\n');
    }
    return v;
}

void flush_stdin(void) { while (getchar() != '\n'); }
void add_circle(void)
{
    if (obj_count >= MAX_OBJECTS) { puts("Object list full!"); return; }
    Shape s;
    memset(&s, 0, sizeof(s));
    s.type   = SHAPE_CIRCLE;
    s.active = 1;
    s.cx = read_int("  Centre column (x): ");
    s.cy = read_int("  Centre row    (y): ");
    s.r  = read_int("  Radius            : ");
    if (s.r <= 0) { puts("Radius must be > 0."); return; }
    objects[obj_count++] = s;
    draw_shape(&s, DRAW);
    printf("  Circle added as object [%d].\n", obj_count - 1);
}

void add_rectangle(void)
{
    if (obj_count >= MAX_OBJECTS) { puts("Object list full!"); return; }
    Shape s;
    memset(&s, 0, sizeof(s));
    s.type   = SHAPE_RECT;
    s.active = 1;
    s.x1 = read_int("  Top-left  column (x1): ");
    s.y1 = read_int("  Top-left  row    (y1): ");
    s.x2 = read_int("  Bot-right column (x2): ");
    s.y2 = read_int("  Bot-right row    (y2): ");
    objects[obj_count++] = s;
    draw_shape(&s, DRAW);
    printf("  Rectangle added as object [%d].\n", obj_count - 1);
}

void add_line(void)
{
    if (obj_count >= MAX_OBJECTS) { puts("Object list full!"); return; }
    Shape s;
    memset(&s, 0, sizeof(s));
    s.type   = SHAPE_LINE;
    s.active = 1;
    s.x1 = read_int("  Start column (x1): ");
    s.y1 = read_int("  Start row    (y1): ");
    s.x2 = read_int("  End   column (x2): ");
    s.y2 = read_int("  End   row    (y2): ");
    objects[obj_count++] = s;
    draw_shape(&s, DRAW);
    printf("  Line added as object [%d].\n", obj_count - 1);
}

void add_triangle(void)
{
    if (obj_count >= MAX_OBJECTS) { puts("Object list full!"); return; }
    Shape s;
    memset(&s, 0, sizeof(s));
    s.type   = SHAPE_TRIANGLE;
    s.active = 1;
    s.x1 = read_int("  Vertex 1 column (x1): ");
    s.y1 = read_int("  Vertex 1 row    (y1): ");
    s.x2 = read_int("  Vertex 2 column (x2): ");
    s.y2 = read_int("  Vertex 2 row    (y2): ");
    s.x3 = read_int("  Vertex 3 column (x3): ");
    s.y3 = read_int("  Vertex 3 row    (y3): ");
    objects[obj_count++] = s;
    draw_shape(&s, DRAW);
    printf("  Triangle added as object [%d].\n", obj_count - 1);
}
void delete_object(void)
{
    list_objects();
    int idx = read_int("Enter object index to delete (-1 to cancel): ");
    if (idx < 0 || idx >= obj_count || !objects[idx].active) {
        puts("Invalid or inactive object index.");
        return;
    }
    objects[idx].active = 0;
    redraw_canvas();
    printf("  Object [%d] deleted.\n", idx);
}
void modify_object(void)
{
    list_objects();
    int idx = read_int("Enter object index to modify (-1 to cancel): ");
    if (idx < 0 || idx >= obj_count || !objects[idx].active) {
        puts("Invalid or inactive object index.");
        return;
    }

    Shape *s = &objects[idx];
    printf("  Modifying %s [%d] – enter new values:\n",
           shape_name(s->type), idx);

    switch (s->type) {
        case SHAPE_CIRCLE:
            s->cx = read_int("  New centre column (x): ");
            s->cy = read_int("  New centre row    (y): ");
            s->r  = read_int("  New radius            : ");
            break;
        case SHAPE_RECT:
            s->x1 = read_int("  New top-left  column (x1): ");
            s->y1 = read_int("  New top-left  row    (y1): ");
            s->x2 = read_int("  New bot-right column (x2): ");
            s->y2 = read_int("  New bot-right row    (y2): ");
            break;
        case SHAPE_LINE:
            s->x1 = read_int("  New start column (x1): ");
            s->y1 = read_int("  New start row    (y1): ");
            s->x2 = read_int("  New end   column (x2): ");
            s->y2 = read_int("  New end   row    (y2): ");
            break;
        case SHAPE_TRIANGLE:
            s->x1 = read_int("  New vertex 1 column (x1): ");
            s->y1 = read_int("  New vertex 1 row    (y1): ");
            s->x2 = read_int("  New vertex 2 column (x2): ");
            s->y2 = read_int("  New vertex 2 row    (y2): ");
            s->x3 = read_int("  New vertex 3 column (x3): ");
            s->y3 = read_int("  New vertex 3 row    (y3): ");
            break;
    }
    redraw_canvas();
    printf("  Object [%d] updated.\n", idx);
}
void clear_all(void)
{
    init_canvas();
    for (int i = 0; i < obj_count; i++)
        objects[i].active = 0;
    obj_count = 0;
    puts("  Canvas and object list cleared.");
}
void print_main_menu(void)
{
    puts("╔══════════════════════════════════════╗");
    puts("║      2D Graphics Editor  (C)         ║");
    puts("╠══════════════════════════════════════╣");
    puts("║  1. Display canvas                   ║");
    puts("║  2. Add shape                        ║");
    puts("║  3. Delete shape                     ║");
    puts("║  4. Modify shape                     ║");
    puts("║  5. List objects                     ║");
    puts("║  6. Clear canvas                     ║");
    puts("║  0. Exit                             ║");
    puts("╚══════════════════════════════════════╝");
    printf("Choice: ");
}

void print_add_menu(void)
{
    puts("\n  --- Add Shape ---");
    puts("  1. Circle");
    puts("  2. Rectangle");
    puts("  3. Line");
    puts("  4. Triangle");
    puts("  0. Back");
    printf("  Choice: ");
}
int main(void)
{
    init_canvas();

    int choice;
    do {
        print_main_menu();
        if (scanf("%d", &choice) != 1) { flush_stdin(); continue; }
        flush_stdin();

        switch (choice) {
            case 1:
                display_canvas();
                break;

            case 2: {
                int sub;
                print_add_menu();
                if (scanf("%d", &sub) != 1) { flush_stdin(); break; }
                flush_stdin();
                switch (sub) {
                    case 1: add_circle();    break;
                    case 2: add_rectangle(); break;
                    case 3: add_line();      break;
                    case 4: add_triangle();  break;
                    case 0: break;
                    default: puts("  Invalid choice.");
                }
                break;
            }

            case 3:
                delete_object();
                break;

            case 4:
                modify_object();
                break;

            case 5:
                list_objects();
                break;

            case 6:
                clear_all();
                break;

            case 0:
                puts("Goodbye!");
                break;

            default:
                puts("Invalid option. Try again.");
        }

    } while (choice != 0);

    return 0;
}



