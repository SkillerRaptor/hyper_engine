/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::entity::Entity;

use std::{
    fmt::Debug,
    slice::{Iter, IterMut},
};

#[derive(Debug)]
pub(crate) struct Entry<T>
where
    T: Debug,
{
    key: Entity,
    value: T,
}

impl<T> Entry<T>
where
    T: Debug,
{
    pub(crate) fn key(&self) -> Entity {
        self.key
    }

    pub(crate) fn value(&self) -> &T {
        &self.value
    }

    pub(crate) fn value_mut(&mut self) -> &mut T {
        &mut self.value
    }
}

#[derive(Debug)]
pub(crate) struct SparseSet<T>
where
    T: Debug,
{
    sparse: Vec<usize>,
    dense: Vec<Entry<T>>,
}

impl<T> SparseSet<T>
where
    T: Debug,
{
    pub(crate) fn new() -> Self {
        Self {
            sparse: Vec::new(),
            dense: Vec::new(),
        }
    }

    pub(crate) fn push(&mut self, entity: Entity, value: T) {
        if self.contains(entity) {
            return;
        }

        let position = self.dense.len();
        self.dense.push(Entry { key: entity, value });

        let entity_id = entity.entity_id() as usize;
        if entity_id >= self.sparse.len() {
            self.sparse.resize(entity_id + 1, usize::MAX);
        }

        self.sparse[entity_id] = position;
    }

    pub(crate) fn remove(&mut self, entity: Entity) {
        if !self.contains(entity) {
            return;
        }

        let entity_id = entity.entity_id() as usize;
        let dense_index = self.sparse[entity_id];

        let last_index = self.dense.len() - 1;
        self.dense.swap(last_index, dense_index);

        let last_dense_index = self.sparse[last_index];
        self.sparse.swap(last_dense_index, dense_index);

        self.dense.pop();
    }

    pub(crate) fn contains(&self, entity: Entity) -> bool {
        let entity_id = entity.entity_id() as usize;
        if entity_id >= self.sparse.len() {
            return false;
        }

        let dense_index = self.sparse[entity_id];
        if dense_index >= self.dense.len() {
            return false;
        }

        self.dense[dense_index].key() == entity
    }

    pub(crate) fn get(&self, entity: Entity) -> Option<&T> {
        if !self.contains(entity) {
            return None;
        }

        let entity_id = entity.entity_id() as usize;
        let dense_index = self.sparse[entity_id];
        let entry = &self.dense[dense_index];
        Some(entry.value())
    }

    pub(crate) fn get_mut(&mut self, entity: Entity) -> Option<&mut T> {
        if !self.contains(entity) {
            return None;
        }

        let entity_id = entity.entity_id() as usize;
        let dense_index = self.sparse[entity_id];
        let entry = &mut self.dense[dense_index];
        Some(entry.value_mut())
    }

    pub(crate) fn clear(&mut self) {
        self.dense.clear();
    }

    pub(crate) fn is_empty(&self) -> bool {
        self.dense.is_empty()
    }

    pub(crate) fn len(&self) -> usize {
        self.dense.len()
    }

    pub(crate) fn capacity(&self) -> usize {
        self.sparse.len()
    }

    pub(crate) fn iter(&self) -> Iter<Entry<T>> {
        self.dense.iter()
    }

    pub(crate) fn iter_mut(&mut self) -> IterMut<Entry<T>> {
        self.dense.iter_mut()
    }
}

impl<T> IntoIterator for SparseSet<T>
where
    T: Debug,
{
    type Item = Entry<T>;
    type IntoIter = std::vec::IntoIter<Self::Item>;

    fn into_iter(self) -> Self::IntoIter {
        self.dense.into_iter()
    }
}

impl<'a, T> IntoIterator for &'a SparseSet<T>
where
    T: Debug,
{
    type Item = &'a Entry<T>;
    type IntoIter = Iter<'a, Entry<T>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a, T> IntoIterator for &'a mut SparseSet<T>
where
    T: Debug,
{
    type Item = &'a mut Entry<T>;
    type IntoIter = IterMut<'a, Entry<T>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter_mut()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn push() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::new(0), 0);
        sparse_set.push(Entity::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.push(Entity::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);
    }

    #[test]
    fn remove() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::new(0), 0);
        sparse_set.push(Entity::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.remove(Entity::new(0));

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 1);
        assert!(!sparse_set.contains(Entity::new(0)));
        assert!(sparse_set.contains(Entity::new(1)));

        sparse_set.remove(Entity::new(1));

        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);
        assert!(!sparse_set.contains(Entity::new(0)));
        assert!(!sparse_set.contains(Entity::new(1)));
    }

    #[test]
    fn contains() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::new(0), 0);
        sparse_set.push(Entity::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        assert!(sparse_set.contains(Entity::new(0)));
        assert!(sparse_set.contains(Entity::new(1)));
    }

    #[test]
    fn get() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::new(0), 0);
        sparse_set.push(Entity::new(1), 1);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        assert!(sparse_set.contains(Entity::new(0)));
        assert!(sparse_set.contains(Entity::new(1)));

        assert_eq!(sparse_set.get(Entity::new(0)), Some(&0));
        assert_eq!(sparse_set.get(Entity::new(1)), Some(&1));
    }

    #[test]
    fn clear() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::new(0), 0);
        sparse_set.push(Entity::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.clear();

        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);
    }

    #[test]
    fn iter() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::new(0), 0);
        sparse_set.push(Entity::new(1), 1);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        for (i, entry) in sparse_set.iter().enumerate() {
            assert_eq!(entry.key(), Entity::new(i as u32));
            assert_eq!(*entry.value(), i as i32);
        }
    }
}
