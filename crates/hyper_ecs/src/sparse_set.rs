/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::{
    fmt::Debug,
    slice::{Iter, IterMut},
};

use hyper_core::handle::Handle;

use crate::entity::Entity;

#[derive(Debug)]
pub struct Entry<T> {
    key: Entity,
    value: T,
}

impl<T> Entry<T> {
    pub fn key(&self) -> Entity {
        self.key
    }

    pub fn value(&self) -> &T {
        &self.value
    }

    pub fn value_mut(&mut self) -> &mut T {
        &mut self.value
    }
}

#[derive(Debug, Default)]
pub struct SparseSet<T> {
    sparse: Vec<usize>,
    dense: Vec<Entry<T>>,
}

impl<T> SparseSet<T> {
    pub fn new() -> Self {
        Self {
            sparse: Vec::new(),
            dense: Vec::new(),
        }
    }

    pub fn push(&mut self, key: Entity, value: T) {
        if self.contains(key) {
            return;
        }

        let position = self.dense.len();
        self.dense.push(Entry { key, value });

        let key_id = key.id() as usize;
        if key_id >= self.sparse.len() {
            self.sparse.resize(key_id + 1, usize::MAX);
        }

        self.sparse[key_id] = position;
    }

    pub fn remove(&mut self, key: Entity) {
        if !self.contains(key) {
            return;
        }

        let key_id = key.id() as usize;
        let dense_index = self.sparse[key_id];

        let last_index = self.dense.len() - 1;
        self.dense.swap(last_index, dense_index);

        let last_dense_index = self.sparse[last_index];
        self.sparse.swap(last_dense_index, dense_index);

        self.dense.pop();
    }

    pub fn contains(&self, key: Entity) -> bool {
        let key_id = key.id() as usize;
        if key_id >= self.sparse.len() {
            return false;
        }

        let dense_index = self.sparse[key_id];
        if dense_index >= self.dense.len() {
            return false;
        }

        self.dense[dense_index].key() == key
    }

    pub fn get(&self, key: Entity) -> Option<&T> {
        if !self.contains(key) {
            return None;
        }

        let key_id = key.id() as usize;
        let dense_index = self.sparse[key_id];
        let entry = &self.dense[dense_index];
        Some(entry.value())
    }

    pub fn get_mut(&mut self, key: Entity) -> Option<&mut T> {
        if !self.contains(key) {
            return None;
        }

        let key_id = key.id() as usize;
        let dense_index = self.sparse[key_id];
        let entry = &mut self.dense[dense_index];
        Some(entry.value_mut())
    }

    pub fn clear(&mut self) {
        self.dense.clear();
    }

    pub fn is_empty(&self) -> bool {
        self.dense.is_empty()
    }

    pub fn len(&self) -> usize {
        self.dense.len()
    }

    pub fn capacity(&self) -> usize {
        self.sparse.len()
    }

    pub fn iter(&self) -> Iter<Entry<T>> {
        self.dense.iter()
    }

    pub fn iter_mut(&mut self) -> IterMut<Entry<T>> {
        self.dense.iter_mut()
    }
}

impl<T> IntoIterator for SparseSet<T> {
    type Item = Entry<T>;
    type IntoIter = std::vec::IntoIter<Self::Item>;

    fn into_iter(self) -> Self::IntoIter {
        self.dense.into_iter()
    }
}

impl<'a, T> IntoIterator for &'a SparseSet<T> {
    type Item = &'a Entry<T>;
    type IntoIter = Iter<'a, Entry<T>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a, T> IntoIterator for &'a mut SparseSet<T> {
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

        sparse_set.push(Entity::from_id(0), 0);
        sparse_set.push(Entity::from_id(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.push(Entity::from_id(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);
    }

    #[test]
    fn remove() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::from_id(0), 0);
        sparse_set.push(Entity::from_id(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.remove(Entity::from_id(0));

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 1);
        assert!(!sparse_set.contains(Entity::from_id(0)));
        assert!(sparse_set.contains(Entity::from_id(1)));

        sparse_set.remove(Entity::from_id(1));

        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);
        assert!(!sparse_set.contains(Entity::from_id(0)));
        assert!(!sparse_set.contains(Entity::from_id(1)));
    }

    #[test]
    fn contains() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::from_id(0), 0);
        sparse_set.push(Entity::from_id(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        assert!(sparse_set.contains(Entity::from_id(0)));
        assert!(sparse_set.contains(Entity::from_id(1)));
    }

    #[test]
    fn get() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::from_id(0), 0);
        sparse_set.push(Entity::from_id(1), 1);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        assert!(sparse_set.contains(Entity::from_id(0)));
        assert!(sparse_set.contains(Entity::from_id(1)));

        assert_eq!(sparse_set.get(Entity::from_id(0)), Some(&0));
        assert_eq!(sparse_set.get(Entity::from_id(1)), Some(&1));
    }

    #[test]
    fn clear() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(Entity::from_id(0), 0);
        sparse_set.push(Entity::from_id(1), 0);

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

        sparse_set.push(Entity::from_id(0), 0);
        sparse_set.push(Entity::from_id(1), 1);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        for (i, entry) in sparse_set.iter().enumerate() {
            assert_eq!(entry.key(), Entity::from_id(i as u32));
            assert_eq!(*entry.value(), i as i32);
        }
    }
}
