// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhijian Yan

#ifndef SGL_LIST_H
#define SGL_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name)      struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *node, struct list_head *prev,
                              struct list_head *next) {
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void list_add(struct list_head *node, struct list_head *head) {
    __list_add(node, head, head->next);
}

static inline void list_add_tail(struct list_head *node,
                                 struct list_head *head) {
    __list_add(node, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = entry;
    entry->prev = entry;
}

static inline int list_is_first(const struct list_head *list,
                                const struct list_head *head) {
    return list->prev == head;
}

static inline int list_is_last(const struct list_head *list,
                               const struct list_head *head) {
    return list->next == head;
}

static inline int list_is_head(const struct list_head *list,
                               const struct list_head *head) {
    return list == head;
}

static inline int list_empty(const struct list_head *head) {
    return head->next == head;
}

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) list_entry((ptr)->prev, type, member)

#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_prev_entry(pos, member) \
    list_entry((pos)->member.prev, typeof(*(pos)), member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)

#define list_for_each_safe(pos, n, head)                                \
    for (pos = (head)->next, n = pos->next; !list_is_head(pos, (head)); \
         pos = n, n = pos->next)

#define list_entry_is_head(pos, head, member) list_is_head(&pos->member, (head))

#define list_for_each_entry(pos, head, member)               \
    for (pos = list_first_entry(head, typeof(*pos), member); \
         !list_entry_is_head(pos, head, member);             \
         pos = list_next_entry(pos, member))

#ifdef __cplusplus
}
#endif

#endif
