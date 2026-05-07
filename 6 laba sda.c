#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

/* ─── Создание узла ─────────────────────────────────────────────────────── */

Node *create_node(int value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }
    new_node->data  = value;
    new_node->left  = NULL;
    new_node->right = NULL;
    return new_node;
}

/* ─── Вставка ───────────────────────────────────────────────────────────── */

Node *insert(Node *root, int value) {
    if (root == NULL)
        return create_node(value);
    if (value < root->data)
        root->left  = insert(root->left,  value);
    else if (value > root->data)
        root->right = insert(root->right, value);
    /* дубликаты игнорируются */
    return root;
}

/* ─── Обходы ────────────────────────────────────────────────────────────── */

void preorder(Node *root) {
    if (root == NULL) return;
    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}

void inorder(Node *root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

void postorder(Node *root) {
    if (root == NULL) return;
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}

void dfs(Node *root) { inorder(root); }

/* ─── BFS (очередь) ─────────────────────────────────────────────────────── */

#define MAX_QUEUE 256

typedef struct {
    Node *items[MAX_QUEUE];
    int   front;
    int   rear;
} Queue;

void  queue_init (Queue *q)          { q->front = q->rear = 0; }
int   queue_empty(Queue *q)          { return q->front == q->rear; }
void  queue_push (Queue *q, Node *n) { q->items[q->rear++ % MAX_QUEUE] = n; }
Node *queue_pop  (Queue *q)          { return q->items[q->front++ % MAX_QUEUE]; }

void bfs(Node *root) {
    if (root == NULL) return;
    Queue q;
    queue_init(&q);
    queue_push(&q, root);
    while (!queue_empty(&q)) {
        Node *cur = queue_pop(&q);
        printf("%d ", cur->data);
        if (cur->left)  queue_push(&q, cur->left);
        if (cur->right) queue_push(&q, cur->right);
    }
}

/* ─── Поиск ─────────────────────────────────────────────────────────────── */

Node *search(Node *root, int key) {
    if (root == NULL)       return NULL;
    if (key == root->data)  return root;
    if (key < root->data)   return search(root->left,  key);
    else                    return search(root->right, key);
}

/* ─── Минимум и максимум ────────────────────────────────────────────────── */

Node *find_min(Node *root) {
    if (root == NULL) return NULL;
    while (root->left != NULL)
        root = root->left;
    return root;
}

Node *find_max(Node *root) {
    if (root == NULL) return NULL;
    while (root->right != NULL)
        root = root->right;
    return root;
}

/* ─── Высота дерева ─────────────────────────────────────────────────────── */

int tree_height(Node *root) {
    if (root == NULL) return 0;
    int left_h  = tree_height(root->left);
    int right_h = tree_height(root->right);
    return 1 + (left_h > right_h ? left_h : right_h);
}

/* ─── Удаление узла ─────────────────────────────────────────────────────── */
/*
 * Три случая:
 *  1. Лист               — просто освобождаем узел.
 *  2. Один потомок       — заменяем узел его единственным потомком.
 *  3. Два потомка        — заменяем значение узла минимальным из правого
 *                          поддерева (inorder-преемник), затем удаляем
 *                          преемника из правого поддерева.
 */
Node *delete_node(Node *root, int key) {
    if (root == NULL) {
        printf("Элемент %d не найден.\n", key);
        return NULL;
    }

    if (key < root->data) {
        root->left  = delete_node(root->left,  key);
    } else if (key > root->data) {
        root->right = delete_node(root->right, key);
    } else {
        /* Узел найден */

        /* Случай 1 & 2: нет левого потомка */
        if (root->left == NULL) {
            Node *tmp = root->right;
            free(root);
            return tmp;
        }
        /* Случай 2: нет правого потомка */
        if (root->right == NULL) {
            Node *tmp = root->left;
            free(root);
            return tmp;
        }
        /* Случай 3: два потомка — ищем inorder-преемника (min в правом) */
        Node *successor   = find_min(root->right);
        root->data        = successor->data;          /* копируем значение */
        root->right       = delete_node(root->right, successor->data);
    }
    return root;
}

/* ─── Освобождение памяти ───────────────────────────────────────────────── */

void free_tree(Node *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* ─── Сохранение / загрузка (текстовый формат, preorder) ───────────────── */
/*
 * Формат файла: числа через пробел, NULL-узлы записываются как символ '#'.
 * Такое представление позволяет точно восстановить структуру дерева.
 */

void save_tree(Node *root, FILE *fp) {
    if (root == NULL) {
        fprintf(fp, "# ");
        return;
    }
    fprintf(fp, "%d ", root->data);
    save_tree(root->left,  fp);
    save_tree(root->right, fp);
}

Node *load_tree(FILE *fp) {
    char buf[32];
    if (fscanf(fp, "%31s", buf) != 1) return NULL;
    if (buf[0] == '#') return NULL;

    int value = atoi(buf);
    Node *node = create_node(value);
    node->left  = load_tree(fp);
    node->right = load_tree(fp);
    return node;
}

/* ─── Меню ──────────────────────────────────────────────────────────────── */

void print_menu(void) {
    printf("\n══════════════════════════════════════\n");
    printf(" 1.  Добавить элемент\n");
    printf(" 2.  Прямой обход      (NLR/Preorder)\n");
    printf(" 3.  Центрированный    (LNR/Inorder)\n");
    printf(" 4.  Обратный обход    (LRN/Postorder)\n");
    printf(" 5.  Обход в глубину   (DFS)\n");
    printf(" 6.  Обход в ширину    (BFS)\n");
    printf(" 7.  Поиск элемента\n");
    printf(" 8.  Удалить элемент\n");
    printf(" 9.  Мин / Макс\n");
    printf(" 10. Высота дерева\n");
    printf(" 11. Сохранить дерево в файл\n");
    printf(" 12. Загрузить дерево из файла\n");
    printf(" 13. Очистить дерево\n");
    printf("  0. Выход\n");
    printf("══════════════════════════════════════\n");
    printf("Выберите пункт: ");
}

/* ─── main ──────────────────────────────────────────────────────────────── */

int main(void) {
    Node *root = NULL;
    int   choice, value;
    char  filename[256];
    FILE *fp;

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {

            /* ── 1. Вставка ─────────────────────────────────────────────── */
            case 1:
                printf("Введите значение: ");
                scanf("%d", &value);
                root = insert(root, value);
                printf("Элемент %d добавлен.\n", value);
                break;

            /* ── 2. Прямой обход ────────────────────────────────────────── */
            case 2:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Прямой обход (NLR): ");
                preorder(root);
                printf("\n");
                break;

            /* ── 3. Центрированный обход ────────────────────────────────── */
            case 3:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Центрированный обход (LNR): ");
                inorder(root);
                printf("\n");
                break;

            /* ── 4. Обратный обход ──────────────────────────────────────── */
            case 4:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Обратный обход (LRN): ");
                postorder(root);
                printf("\n");
                break;

            /* ── 5. DFS ─────────────────────────────────────────────────── */
            case 5:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Обход в глубину (DFS): ");
                dfs(root);
                printf("\n");
                break;

            /* ── 6. BFS ─────────────────────────────────────────────────── */
            case 6:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Обход в ширину (BFS): ");
                bfs(root);
                printf("\n");
                break;

            /* ── 7. Поиск ───────────────────────────────────────────────── */
            case 7:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Введите ключ для поиска: ");
                scanf("%d", &value);
                if (search(root, value))
                    printf("Элемент %d НАЙДЕН.\n", value);
                else
                    printf("Элемент %d НЕ НАЙДЕН.\n", value);
                break;

            /* ── 8. Удаление ────────────────────────────────────────────── */
            case 8:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Введите значение для удаления: ");
                scanf("%d", &value);
                root = delete_node(root, value);
                printf("Готово.\n");
                break;

            /* ── 9. Мин / Макс ──────────────────────────────────────────── */
            case 9:
                if (!root) { printf("Дерево пустое.\n"); break; }
                printf("Минимум: %d\n", find_min(root)->data);
                printf("Максимум: %d\n", find_max(root)->data);
                break;

            /* ── 10. Высота ─────────────────────────────────────────────── */
            case 10:
                printf("Высота дерева: %d\n", tree_height(root));
                break;

            /* ── 11. Сохранение ─────────────────────────────────────────── */
            case 11:
                printf("Имя файла для сохранения: ");
                scanf("%255s", filename);
                fp = fopen(filename, "w");
                if (!fp) { perror("fopen"); break; }
                save_tree(root, fp);
                fclose(fp);
                printf("Дерево сохранено в '%s'.\n", filename);
                break;

            /* ── 12. Загрузка ───────────────────────────────────────────── */
            case 12:
                printf("Имя файла для загрузки: ");
                scanf("%255s", filename);
                fp = fopen(filename, "r");
                if (!fp) { perror("fopen"); break; }
                free_tree(root);          /* освобождаем текущее дерево */
                root = load_tree(fp);
                fclose(fp);
                printf("Дерево загружено из '%s'.\n", filename);
                break;

            /* ── 13. Очистка ────────────────────────────────────────────── */
            case 13:
                free_tree(root);
                root = NULL;
                printf("Дерево очищено.\n");
                break;

            /* ── 0. Выход ───────────────────────────────────────────────── */
            case 0:
                printf("Выход.\n");
                free_tree(root);
                return 0;

            default:
                printf("Неверный пункт. Попробуйте снова.\n");
        }
    }

    free_tree(root);
    return 0;
}