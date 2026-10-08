// SPDX-License-Identifier: MIT
// Copyright (c) 2025-2026 Zhijian Yan

#ifndef SGL_LIST_H
#define SGL_LIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define sgl_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

struct sgl_list_head {
    struct sgl_list_head *next;
    struct sgl_list_head *prev;
};

#define SGL_LIST_HEAD_INIT(name) {&(name), &(name)}
#define SGL_LIST_HEAD(name)      struct sgl_list_head name = SGL_LIST_HEAD_INIT(name)

static inline void sgl_init_list_head(struct sgl_list_head *list) {
    list->next = list;
    list->prev = list;
}

static inline void sgl_list_add_internal(struct sgl_list_head *node,
                                         struct sgl_list_head *prev,
                                         struct sgl_list_head *next) {
    next->prev = node;
    node->next = next;
    node->prev = prev;
    prev->next = node;
}

static inline void sgl_list_add(struct sgl_list_head *node,
                                struct sgl_list_head *head) {
    sgl_list_add_internal(node, head, head->next);
}

static inline void sgl_list_add_tail(struct sgl_list_head *node,
                                     struct sgl_list_head *head) {
    sgl_list_add_internal(node, head->prev, head);
}

static inline void sgl_list_del_internal(struct sgl_list_head *prev,
                                         struct sgl_list_head *next) {
    next->prev = prev;
    prev->next = next;
}

static inline void sgl_list_del(struct sgl_list_head *entry) {
    sgl_list_del_internal(entry->prev, entry->next);
    entry->next = entry;
    entry->prev = entry;
}

static inline int sgl_list_is_first(const struct sgl_list_head *list,
                                    const struct sgl_list_head *head) {
    return list->prev == head;
}

static inline int sgl_list_is_last(const struct sgl_list_head *list,
                                   const struct sgl_list_head *head) {
    return list->next == head;
}

static inline int sgl_list_is_head(const struct sgl_list_head *list,
                                   const struct sgl_list_head *head) {
    return list == head;
}

static inline int sgl_list_empty(const struct sgl_list_head *head) {
    return head->next == head;
}

#define sgl_list_entry(ptr, type, member) sgl_container_of(ptr, type, member)

#define sgl_list_first_entry(ptr, type, member) \
    sgl_list_entry((ptr)->next, type, member)

#define sgl_list_last_entry(ptr, type, member) \
    sgl_list_entry((ptr)->prev, type, member)

#define sgl_list_next_entry(pos, member) \
    sgl_list_entry((pos)->member.next, typeof(*(pos)), member)

#define sgl_list_prev_entry(pos, member) \
    sgl_list_entry((pos)->member.prev, typeof(*(pos)), member)

#define sgl_list_for_each(pos, head) \
    for (pos = (head)->next; !sgl_list_is_head(pos, (head)); pos = pos->next)

#define sgl_list_for_each_safe(pos, n, head)                                \
    for (pos = (head)->next, n = pos->next; !sgl_list_is_head(pos, (head)); \
         pos = n, n = pos->next)

#define sgl_list_entry_is_head(pos, head, member) \
    sgl_list_is_head(&pos->member, (head))

#define sgl_list_for_each_entry(pos, head, member)               \
    for (pos = sgl_list_first_entry(head, typeof(*pos), member); \
         !sgl_list_entry_is_head(pos, head, member);             \
         pos = sgl_list_next_entry(pos, member))

#ifdef __cplusplus
}
#endif

#endif
