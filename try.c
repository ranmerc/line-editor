//Kamran Ansari
//TY BSC CS SEC A
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char *file_name = NULL;
long file_size = 0;
long line_count = 0;

struct line
{
    char *data;
    struct line *next_line;
    struct line *prev_line;
};

struct line *start = NULL;

void help(char command);                         //done
void print_file(char command[10][20], int num);  //done
int open_file(char *fn);                         //done
void save_file(char command[10][20], int num);   //done
void new_file(char command[10][20], int num);    //done
void append_line(char command[10][20], int num); //done but adds a new line also cannot insert empty line
void insert_line(char command[10][20], int num); //done not tested
void update_line(char command[10][20], int num);  //done not tested
void copy_line(char command[10][20], int num);   //done and seriously not tested
void move_line(char command[10][20], int num);   //done and seriously not tested
void delete_line(char command[10][20], int num); //done not tested
void console();                                  //done
void parse_command(char *);                      //done
void quit_console(int i);                        //done

int isNumber(char *str)
{
    int i;
    for (i = 0; i < strlen(str); i++)
        if (!isdigit(str[i]))
            return 0;
    return 1;
}

void console()
{
    char *curpath, *string;
    if ((curpath = getcwd(NULL, 0)) != NULL)
        printf("\nLine Editor @ %s\\%s\n=> ", curpath, file_name ? file_name : "");
    else
        printf("Line Editor \n=> ");
    scanf(" %1024[^\n]", string);
    parse_command(string);
}

void parse_command(char *str)
{
    char command[10][20];
    char *ptr;
    int i = 0;
    for (ptr = strtok(str, " "); ptr != NULL; ptr = strtok(NULL, " "), ++i)
        strcpy(command[i], ptr);
    if (i > 4)
    {
        printf("\nE : TOO MANY ARGUMENTS");
        help('z');
        return;
    }
    if (strlen(command[0]) > 1)
    {
        printf("\nE : UNKNOWN COMMAND %s\n", command[0]);
        help('z');
        return;
    }
    switch (command[0][0])
    {
    case 'h':
        if (i - 1 > 1 || strlen(command[1]) > 1)
        {
            printf("\nE : INVLAID ARGUMENTS\n");
            return;
        }
        help(command[1][0]);
        break;
    case 'p':
        print_file(command, i - 1);
        break;
    case 'a':
        append_line(command, i - 1);
        break;
    case 'i':
        insert_line(command, i - 1);
        break;
    case 's':
        save_file(command, i - 1);
        break;
    case 'u':
        update_line(command, i - 1);
        break;
    case 'n':
        new_file(command, i - 1);
        break;
    case 'o':
        if (i == 2)
            open_file(command[1]);
        else
        {
            printf("\nE : INVALID NUMBER OF ARGUMENTS\n");
            help('o');
        }
        break;
    case 'm':
        move_line(command, i - 1);
        break;
    case 'c':
        copy_line(command, i - 1);
        break;
    case 'd':
        delete_line(command, i - 1);
        break;
    case 'q':
        quit_console(i - 1);
        break;
    default:
        printf("E: UNKNOWN COMMAND!\n");
        help('z');
    }
}

int open_file(char *fn)
{
    if (access(fn, F_OK) == -1)
    {
        printf("\nE : FILE %s DOES NOT EXIST\n", fn);
        return 0;
    }
    file_name = malloc(sizeof(char) * 255);
    strcpy(file_name, fn);
    FILE *fp;
    fp = fopen(file_name, "r");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    if (!file_size)
        return 1;
    fseek(fp, 0, SEEK_SET);
    char *buff = malloc(sizeof(char) * 1024);
    char *res;
    start = NULL;
    line_count = 0;
    while ((res = fgets(buff, 1024, fp)) != NULL)
    {
        ++line_count;
        struct line *new = (struct line *)malloc(sizeof(struct line));
        new->data = malloc(sizeof(char) * 1024);
        strcpy(new->data, res);
        new->next_line = NULL;
        new->prev_line = NULL;
        if (start == NULL)
            start = new;
        else
        {
            struct line *curr = start;
            while (curr->next_line != NULL)
                curr = curr->next_line;
            curr->next_line = new;
            new->prev_line = curr;
        }
    }
    fclose(fp);
    return 1;
}

void print_file(char command[10][20], int num)
{
    int n = 1, m = line_count;
    if (!num)
    {
        if (file_name == NULL)
        {
            printf("E : NO FILE CURRENTLY OPENED\n");
            return;
        }
        if (start == NULL && line_count <= 0)
        {
            printf("W : FILE EMPTY!\n");
            return;
        }
    }
    else if (!isNumber(command[1]))
    {
        if ((!open_file(command[1])))
            return;
        if (num == 2 && isNumber(command[2]))
        {
            m = atoi(command[2]);
        }
        else if (num == 3 && isNumber(command[2]) && isNumber(command[3]))
        {
            n = atoi(command[2]);
            m = atoi(command[3]);
        }
        else
        {
            m = line_count;
        }
    }
    else if (num == 1)
        m = atoi(command[1]);
    else if (num == 2)
    {
        n = atoi(command[1]);
        m = atoi(command[2]);
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('p');
        return;
    }
    if (file_name == NULL)
    {
        printf("E : NO FILE CURRENTLY OPENED\n");
        return;
    }
    if (line_count <= 0)
    {
        printf("\nW : FILE EMPTY!\n");
        return;
    }
    if (m < 0 || n > m || n < 0)
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('p');
        return;
    }
    if (n > line_count)
    {
        printf("\nE : INDEX OUT OF BOUND\n");
        return;
    }
    if (m > line_count)
        m = line_count;
    struct line *curr = start;
    int k;
    for (k = n; k > 1; --k)
        curr = curr->next_line;
    for (k = m - n; k >= 0; --k)
    {
        printf("%s", curr->data);
        curr = curr->next_line;
    }
}

void help(char command)
{
    FILE *fp = fopen("help.txt", "r");
    if (!fp)
    {
        printf("\nHelp Not Available!");
        printf("\nE : help.txt MISSING\n");
    }
    char ch;
    switch (command)
    {
    case 'p':
        fseek(fp, 467, SEEK_SET);
        break;
    case 'a':
        fseek(fp, 719, SEEK_SET);
        break;
    case 'i':
        fseek(fp, 850, SEEK_SET);
        break;
    case 's':
        fseek(fp, 963, SEEK_SET);
        break;
    case 'u':
        fseek(fp, 1127, SEEK_SET);
        break;
    case 'n':
        fseek(fp, 1234, SEEK_SET);
        break;
    case 'o':
        fseek(fp, 1327, SEEK_SET);
        break;
    case 'm':
        fseek(fp, 1463, SEEK_SET);
        break;
    case 'c':
        fseek(fp, 1640, SEEK_SET);
        break;
    case 'd':
        fseek(fp, 1805, SEEK_SET);
        break;
    case 'q':
        fseek(fp, 1931, SEEK_SET);
        break;
    }
    printf("\n");
    while ((ch = fgetc(fp)) != '~')
        printf("%c", ch);
}

void save_file(char command[10][20], int num)
{
    char *fn = (char *)malloc(sizeof(char) * 255);
    if (!num)
    {
        if (file_name == NULL)
        {
            printf("\n NO FILE CURRENTLY OPENED\n");
            return;
        }
        strcpy(fn, file_name);
    }
    else if (num == 1 && !isNumber(command[1]))
    {
        strcpy(fn, command[1]);
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('s');
    }
    FILE *fout = fopen(fn, "w");
    if (fout == NULL)
    {
        printf("\nE : NOT ABLE TO SAVE THE FILE\n");
        return;
    }
    struct line *temp = start;
    while (temp != NULL)
    {
        fprintf(fout, "%s", temp->data);
        temp = temp->next_line;
    }
    fclose(fout);
    return;
}

void new_file(char command[10][20], int num)
{
    if (num != 1 || isNumber(command[1]))
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('n');
        return;
    }
    if (access(command[1], F_OK) != -1)
    {
        printf("\nE : FILE %s ALREADY EXISTS\n", command[1]);
        return;
    }
    FILE *fp = fopen(command[1], "w");
    if (fp == NULL)
    {
        printf("\nE : UNABLE TO CREATE FILE\n");
        return;
    }
    open_file(command[1]);
}

void append_line(char command[10][20], int num)
{
    //problem is that when we insert from program it adds \n
    if (!num)
    {
        if (file_name == NULL)
        {
            printf("\nE : NO FILE CURRENTLY OPENED\n");
            return;
        }
    }
    else if (num == 1 && !isNumber(command[1]))
    {
        if (access(command[1], F_OK) == -1)
        {
            printf("\nE : FILE DOES NOT EXISTS\n");
            return;
        }
        open_file(command[1]);
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('a');
        return;
    }
    char str[1024];
    printf("\nEnter Your Text (~ to end)- \n");
    scanf(" %[^~]s", str);
    char *ptr;
    for (ptr = strtok(str, "\n"); ptr != NULL; ptr = strtok(NULL, "\n"))
    {
        ++line_count;
        struct line *new = (struct line *)malloc(sizeof(struct line));
        new->data = (char *)malloc(sizeof(char) * 1024);
        strcpy(new->data, ptr);
        strcat(new->data, "\n");
        new->next_line = NULL;
        new->prev_line = NULL;
        if (start == NULL)
            start = new;
        else
        {
            struct line *curr = start;
            while (curr->next_line != NULL)
                curr = curr->next_line;
            curr->next_line = new;
            new->prev_line = curr;
        }
    }
    fflush(stdin);
}

void insert_line(char command[10][20], int num)
{
    if (!num)
        append_line(command, num);
    else if (num == 1 && isNumber(command[1]))
    {
        int n;
        if (file_name == NULL)
        {
            printf("\nE : NO FILE CURRENTLY OPENED\n");
            return;
        }
        n = atoi(command[1]);
        if (n >= line_count)
        {
            append_line(command, 0);
            return;
        }
        char str[1024];
        printf("\nEnter Your Text (~ to end)- \n");
        scanf(" %[^~]s", str);
        char *ptr;
        struct line *temp = start;
        int i;
        for (i = n; i > 1; --i)
            temp = temp->next_line;
        struct line *next = temp->next_line;
        for (ptr = strtok(str, "\n"); ptr != NULL; ptr = strtok(NULL, "\n"))
        {
            ++line_count;
            struct line *new = (struct line *)malloc(sizeof(struct line));
            new->data = malloc(sizeof(char) * 1024);
            strcpy(new->data, ptr);
            strcat(new->data, "\n");
            new->next_line = NULL;
            new->prev_line = NULL;
            temp->next_line = new;
            temp = new;
        }
        temp->next_line = next;
        fflush(stdin);
    }
    else
        printf("\nE : INVALID ARGUMENTS\n");
}

void update_line(char command[10][20], int num)
{
    if (num == 1 && isNumber(command[1]))
    {
        int n;
        if (file_name == NULL)
        {
            printf("\nE : NO FILE CURRENTLY OPENED\n");
            return;
        }
        n = atoi(command[1]);
        if (n > line_count)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        int k = n;
        struct line *temp = start;
        for (; k > 1; --k)
            temp = temp->next_line;
        char str[1024];
        printf("\nEnter Your Text - \n");
        scanf(" %[^\n]s", str);
        strcpy(temp->data, str);
        strcat(temp->data, "\n");
        fflush(stdin);
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('u');
    }
}

void copy_line(char command[10][20], int num)
{
    int k, n, m;
    if (file_name == NULL)
    {
        printf("\nE : NO FILE CURRENTLY OPENED\n");
        return;
    }
    if (line_count <= 0)
    {
        printf("\nE : FILE IS EMPTY\n");
        return;
    }
    if (num == 2 && isNumber(command[1]) && isNumber(command[2]))
    {
        n = atoi(command[1]);
        m = atoi(command[2]);
        if (n <= 0 || m <= 0)
        {
            printf("\nE : INVALID ARGUMENTS\n");
            return;
        }
        if (n > line_count || m > line_count)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        struct line *temp = start;
        for (k = n; k > 1; --k)
            temp = temp->next_line;
        struct line *copy = temp;
        temp = start;
        for (k = m; k > 1; --k)
            temp = temp->next_line;
        struct line *new = (struct line *)malloc(sizeof(struct line));
        new->data = (char *)malloc(sizeof(char) * 1024);
        strcpy(new->data, copy->data);
        new->prev_line = NULL;
        new->next_line = NULL;
        if (temp == start)
        {
            new->next_line = start;
            start->prev_line = new;
            start = new;
        }
        else
        {
            new->prev_line = temp->prev_line;
            new->next_line = temp;
            temp->prev_line->next_line = new;
            temp->prev_line = new;
        }
        ++line_count;
    }
    else if (num == 3 && isNumber(command[1]) && isNumber(command[2]) && isNumber(command[3]))
    {
        n = atoi(command[1]);
        m = atoi(command[2]);
        k = atoi(command[3]);
        if (n <= 0 || m <= 0 || k <= 0 || n >= m)
        {
            printf("\nE : INVALID ARGUMENTS\n");
            return;
        }
        if (n > line_count || m > line_count || k > line_count)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        struct line *from = start;
        int i;
        for (i = n; i > 1; --i)
            from = from->next_line;
        struct line *to = start;
        for (i = m; i > 1; --i)
            to = to->next_line;
        struct line *put = start;
        for (i = k; i > 1; --i)
            put = put->next_line;
        struct line *iter;
        char *str[50];
        for (iter = from, i = 0; iter != to->next_line; iter = iter->next_line, i++)
        {
            str[i] = (char *)malloc(sizeof(char) * 1024);
            strcpy(str[i], iter->data);
        }
        int l;
        for (l = 0; l < i; l++)
        {
            ++line_count;
            struct line *new = (struct line *)malloc(sizeof(struct line));
            new->data = (char *)malloc(sizeof(char) * 1024);
            strcpy(new->data, str[l]);
            new->prev_line = NULL;
            new->next_line = NULL;
            if (put == start)
            {
                new->next_line = start;
                start->prev_line = new;
                start = new;
            }
            else
            {
                new->prev_line = put->prev_line;
                new->next_line = put;
                put->prev_line->next_line = new;
                put->prev_line = new;
            }
        }
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('c');
    }
}

void move_line(char command[10][20], int num)
{
    int k, n, m;
    if (file_name == NULL)
    {
        printf("\nE : NO FILE CURRENTLY OPENED\n");
        return;
    }
    if (line_count <= 0)
    {
        printf("\nE : FILE IS EMPTY\n");
        return;
    }
    if (num == 2 && isNumber(command[1]) && isNumber(command[2]))
    {
        n = atoi(command[1]);
        m = atoi(command[2]);
        if (n <= 0 || m <= 0)
        {
            printf("\nE : INVALID ARGUMENTS\n");
            return;
        }
        if (n > line_count || m > line_count)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        struct line *temp = start, *move;
        for (k = n; k > 1; --k)
            temp = temp->next_line;
        move = temp;
        //deletion
        if (temp == start)
            start = temp->next_line;
        if (temp->next_line != NULL)
            temp->next_line->prev_line = temp->prev_line;
        if (temp->prev_line != NULL)
            temp->prev_line->next_line = temp->next_line;
        temp = start;
        for (k = m; k > 1; --k)
            temp = temp->next_line;
        //same as copying
        if (temp == start)
        {
            move->next_line = start;
            start->prev_line = move;
            start = move;
        }
        else
        {
            move->prev_line = temp->prev_line;
            move->next_line = temp;
            temp->prev_line->next_line = move;
            temp->prev_line = move;
        }
    }
    else if (num == 3 && isNumber(command[1]) && isNumber(command[2]) && isNumber(command[3]))
    {
        n = atoi(command[1]);
        m = atoi(command[2]);
        k = atoi(command[3]);
        if (n <= 0 || m <= 0 || k <= 0 || n >= m)
        {
            printf("\nE : INVALID ARGUMENTS\n");
            return;
        }
        if (n > line_count || m > line_count || k > line_count)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        int l;
        struct line *from, *to, *put;
        from = to = put = start;
        for (l = n; l > 1; --l)
            from = from->next_line;
        for (l = m; l > 1; --l)
            to = to->next_line;
        for (l = k; l > 1; --l)
            put = put->next_line;
        if (from == start)
            start = to->next_line;
        else
            from->prev_line->next_line = to->next_line;
        to->next_line->prev_line = from->prev_line;
        put->prev_line->next_line = from;
        from->prev_line = put->prev_line;
        put->prev_line = to;
        to->next_line = put;
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('m');
    }
}

void delete_line(char command[10][20], int num)
{
    int n, m;
    if (file_name == NULL)
    {
        printf("\nE : NO FILE CURRENTLY OPENED\n");
        return;
    }
    if (num == 1 && isNumber(command[1]))
    {
        n = atoi(command[1]);
        if (line_count <= 0)
        {
            printf("\nE : FILE ALREADY EMPTY\n");
            return;
        }
        if (n > line_count || n < 1)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        struct line *temp = start;
        for (; n > 1; --n)
            temp = temp->next_line;
        if (temp == start)
            start = temp->next_line;
        if (temp->next_line != NULL)
            temp->next_line->prev_line = temp->prev_line;
        if (temp->prev_line != NULL)
            temp->prev_line->next_line = temp->next_line;
        --line_count;
        free(temp);
    }
    else if (num == 2 && isNumber(command[1]) && isNumber(command[2]))
    {
        if (line_count <= 0)
        {
            printf("\nE : FILE ALREADY EMPTY\n");
            return;
        }
        n = atoi(command[1]);
        m = atoi(command[2]);
        if (m < 0 || n > m || n < 0)
        {
            printf("\nE : INVALID ARGUMENTS\n");
            help('d');
            return;
        }
        if (n > line_count)
        {
            printf("\nE : INDEX OUT OF BOUND\n");
            return;
        }
        if (m > line_count)
            m = line_count;
        struct line *temp = start;
        struct line *del;
        int i;
        for (i = n; i > 1; --i)
            temp = temp->next_line;
        for (i = m - n; i >= 0; --i)
        {
            del = temp;
            temp = temp->next_line;
            if (del == start)
                start = del->next_line;
            if (del->next_line != NULL)
                del->next_line->prev_line = del->prev_line;
            if (del->prev_line != NULL)
                del->prev_line->next_line = del->next_line;
            --line_count;
            free(del);
        }
    }
    else
    {
        printf("\nE : INVALID ARGUMENTS\n");
        help('d');
        return;
    }
}

void quit_console(int n)
{
    if (n)
    {
        printf("\nE : TOO MANY ARGUMENTS");
        help('q');
        return;
    }
    exit(0);
}

int main()
{
    while (1)
    {
        console();
    }
    return 0;
}