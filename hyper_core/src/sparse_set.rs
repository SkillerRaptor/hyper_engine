/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use num_traits::PrimInt;

use crate::handle::Handle;

use std::{
    fmt::Debug,
    marker::PhantomData,
    slice::{Iter, IterMut},
};

#[derive(Debug)]
pub struct Entry<K, T, A, B>
where
    K: Handle<A, B>,
    A: PrimInt,
    B: PrimInt,
{
    key: K,
    value: T,
    _marker: PhantomData<(K, T, A, B)>,
}

impl<K, T, A, B> Entry<K, T, A, B>
where
    K: Handle<A, B>,
    T: Debug,
    A: PrimInt,
    B: PrimInt,
{
    pub fn key(&self) -> K {
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
pub struct SparseSet<K, T, A, B>
where
    K: Handle<A, B>,
    A: PrimInt,
    B: PrimInt,
{
    sparse: Vec<usize>,
    dense: Vec<Entry<K, T, A, B>>,
    _marker: PhantomData<(K, T, A, B)>,
}

impl<K, T, A, B> SparseSet<K, T, A, B>
where
    K: Handle<A, B>,
    T: Debug,
    A: PrimInt,
    B: PrimInt,
{
    pub fn new() -> Self {
        Self {
            sparse: Vec::new(),
            dense: Vec::new(),
            _marker: PhantomData,
        }
    }

    pub fn push(&mut self, key: K, value: T) {
        if self.contains(key) {
            return;
        }

        let position = self.dense.len();
        self.dense.push(Entry {
            key,
            value,
            _marker: PhantomData,
        });

        let key_id = key.handle().to_usize().unwrap();
        if key_id >= self.sparse.len() {
            self.sparse.resize(key_id + 1, usize::MAX);
        }

        self.sparse[key_id] = position;
    }

    pub fn remove(&mut self, key: K) {
        if !self.contains(key) {
            return;
        }

        let key_id = key.handle().to_usize().unwrap();
        let dense_index = self.sparse[key_id];

        let last_index = self.dense.len() - 1;
        self.dense.swap(last_index, dense_index);

        let last_dense_index = self.sparse[last_index];
        self.sparse.swap(last_dense_index, dense_index);

        self.dense.pop();
    }

    pub fn contains(&self, key: K) -> bool {
        let key_id = key.handle().to_usize().unwrap();
        if key_id >= self.sparse.len() {
            return false;
        }

        let dense_index = self.sparse[key_id];
        if dense_index >= self.dense.len() {
            return false;
        }

        self.dense[dense_index].key() == key
    }

    pub fn get(&self, key: K) -> Option<&T> {
        if !self.contains(key) {
            return None;
        }

        let key_id = key.handle().to_usize().unwrap();
        let dense_index = self.sparse[key_id];
        let entry = &self.dense[dense_index];
        Some(entry.value())
    }

    pub fn get_mut(&mut self, key: K) -> Option<&mut T> {
        if !self.contains(key) {
            return None;
        }

        let key_id = key.handle().to_usize().unwrap();
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

    pub fn iter(&self) -> Iter<Entry<K, T, A, B>> {
        self.dense.iter()
    }

    pub fn iter_mut(&mut self) -> IterMut<Entry<K, T, A, B>> {
        self.dense.iter_mut()
    }
}

impl<K, T, A, B> IntoIterator for SparseSet<K, T, A, B>
where
    K: Handle<A, B>,
    T: Debug,
    A: PrimInt,
    B: PrimInt,
{
    type Item = Entry<K, T, A, B>;
    type IntoIter = std::vec::IntoIter<Self::Item>;

    fn into_iter(self) -> Self::IntoIter {
        self.dense.into_iter()
    }
}

impl<'a, K, T, A, B> IntoIterator for &'a SparseSet<K, T, A, B>
where
    K: Handle<A, B>,
    T: Debug,
    A: PrimInt,
    B: PrimInt,
{
    type Item = &'a Entry<K, T, A, B>;
    type IntoIter = Iter<'a, Entry<K, T, A, B>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a, K, T, A, B> IntoIterator for &'a mut SparseSet<K, T, A, B>
where
    K: Handle<A, B>,
    T: Debug,
    A: PrimInt,
    B: PrimInt,
{
    type Item = &'a mut Entry<K, T, A, B>;
    type IntoIter = IterMut<'a, Entry<K, T, A, B>>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter_mut()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[repr(transparent)]
    #[derive(Clone, Copy, Debug, PartialEq, Eq)]
    pub struct TestHandle(pub u64);

    impl TestHandle {
        pub fn new(id: u32) -> Self {
            Self((1 << Self::half_shift()) | id as u64)
        }
    }

    impl Default for TestHandle {
        fn default() -> Self {
            Self::new(u32::MAX)
        }
    }

    impl Handle<u64, u32> for TestHandle {
        fn create(handle: u32) -> Self {
            Self::new(handle)
        }

        fn value(&self) -> u64 {
            self.0
        }

        fn value_mut(&mut self) -> &mut u64 {
            &mut self.0
        }
    }

    #[test]
    fn push() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(TestHandle::new(0), 0);
        sparse_set.push(TestHandle::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.push(TestHandle::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);
    }

    #[test]
    fn remove() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(TestHandle::new(0), 0);
        sparse_set.push(TestHandle::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        sparse_set.remove(TestHandle::new(0));

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 1);
        assert!(!sparse_set.contains(TestHandle::new(0)));
        assert!(sparse_set.contains(TestHandle::new(1)));

        sparse_set.remove(TestHandle::new(1));

        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);
        assert!(!sparse_set.contains(TestHandle::new(0)));
        assert!(!sparse_set.contains(TestHandle::new(1)));
    }

    #[test]
    fn contains() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(TestHandle::new(0), 0);
        sparse_set.push(TestHandle::new(1), 0);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        assert!(sparse_set.contains(TestHandle::new(0)));
        assert!(sparse_set.contains(TestHandle::new(1)));
    }

    #[test]
    fn get() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(TestHandle::new(0), 0);
        sparse_set.push(TestHandle::new(1), 1);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        assert!(sparse_set.contains(TestHandle::new(0)));
        assert!(sparse_set.contains(TestHandle::new(1)));

        assert_eq!(sparse_set.get(TestHandle::new(0)), Some(&0));
        assert_eq!(sparse_set.get(TestHandle::new(1)), Some(&1));
    }

    #[test]
    fn clear() {
        let mut sparse_set = SparseSet::new();
        assert!(sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 0);

        sparse_set.push(TestHandle::new(0), 0);
        sparse_set.push(TestHandle::new(1), 0);

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

        sparse_set.push(TestHandle::new(0), 0);
        sparse_set.push(TestHandle::new(1), 1);

        assert!(!sparse_set.is_empty());
        assert_eq!(sparse_set.len(), 2);

        for (i, entry) in sparse_set.iter().enumerate() {
            assert_eq!(entry.key(), TestHandle::new(i as u32));
            assert_eq!(*entry.value(), i as i32);
        }
    }
}
