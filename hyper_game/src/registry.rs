/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{entity::Entity, sparse_set::SparseSet};

use hyper_core::handle_manager::HandleManager;

use std::{
    any::{Any, TypeId},
    collections::HashMap,
    fmt::Debug,
};

pub(crate) trait ComponentList {
    fn as_any(&self) -> &dyn Any;

    fn as_any_mut(&mut self) -> &mut dyn Any;
}

impl<T> ComponentList for SparseSet<T>
where
    T: 'static + Debug,
{
    fn as_any(&self) -> &dyn Any {
        self
    }

    fn as_any_mut(&mut self) -> &mut dyn Any {
        self
    }
}

#[derive(Default)]
pub struct Registry {
    pub handle_manager: HandleManager<Entity>,

    components: Vec<Box<dyn ComponentList>>,
    component_indices: HashMap<TypeId, usize>,
}

impl Registry {
    pub fn new() -> Self {
        Self {
            handle_manager: HandleManager::new(),

            components: Vec::new(),
            component_indices: HashMap::new(),
        }
    }

    pub fn create_entity(&mut self) -> Entity {
        self.handle_manager.create_handle()
    }

    pub fn destroy_entity(&mut self, entity: Entity) {
        self.handle_manager.destroy_handle(entity)
    }

    pub fn add_component<T: 'static + Debug>(&mut self, entity: Entity, component: T) {
        assert!(self.handle_manager.is_handle_valid(entity));

        let component_id = self.get_component_id_or_construct::<T>();
        let component_set = &mut self.components[component_id];

        let specific_set = component_set
            .as_any_mut()
            .downcast_mut::<SparseSet<T>>()
            .unwrap();
        if specific_set.contains(entity) {
            return;
        }

        specific_set.push(entity, component);
    }

    pub fn remove_component<T: 'static + Debug>(&mut self, entity: Entity) {
        assert!(self.handle_manager.is_handle_valid(entity));

        let component_id = self.get_component_id_or_construct::<T>();
        let component_set = &mut self.components[component_id];

        let specific_set = component_set
            .as_any_mut()
            .downcast_mut::<SparseSet<T>>()
            .unwrap();
        if !specific_set.contains(entity) {
            return;
        }

        specific_set.remove(entity);
    }

    pub fn get_component<T: 'static + Debug>(&self, entity: Entity) -> Option<&T> {
        assert!(self.handle_manager.is_handle_valid(entity));

        let Some(component_id) = self.get_component_id::<T>() else {
            return None;
        };

        let component_set = &self.components[component_id];

        let specific_set = component_set
            .as_any()
            .downcast_ref::<SparseSet<T>>()
            .unwrap();
        if !specific_set.contains(entity) {
            return None;
        }

        specific_set.get(entity)
    }

    pub fn get_component_mut<T: 'static + Debug>(&mut self, entity: Entity) -> Option<&mut T> {
        assert!(self.handle_manager.is_handle_valid(entity));

        let component_id = self.get_component_id_or_construct::<T>();
        let component_set = &mut self.components[component_id];

        let specific_set = component_set
            .as_any_mut()
            .downcast_mut::<SparseSet<T>>()
            .unwrap();
        if !specific_set.contains(entity) {
            return None;
        }

        specific_set.get_mut(entity)
    }

    pub fn view_one<F, A>(&self, mut f: F)
    where
        A: 'static + Debug,
        F: FnMut(Entity, &A),
    {
        let Some(component_id) = self.get_component_id::<A>() else {
            return;
        };

        let component_set = &self.components[component_id];
        let specific_set = component_set
            .as_any()
            .downcast_ref::<SparseSet<A>>()
            .unwrap();

        for entry in specific_set.iter() {
            f(entry.key(), entry.value());
        }
    }

    pub fn view_one_mut<F, A>(&mut self, mut f: F)
    where
        A: 'static + Debug,
        F: FnMut(Entity, &mut A),
    {
        let Some(component_id) = self.get_component_id::<A>() else {
            return;
        };

        let component_set = &mut self.components[component_id];
        let specific_set = component_set
            .as_any_mut()
            .downcast_mut::<SparseSet<A>>()
            .unwrap();

        for entry in specific_set.iter_mut() {
            f(entry.key(), entry.value_mut());
        }
    }

    pub fn view_two<F, A, B>(&self, mut f: F)
    where
        A: 'static + Debug,
        B: 'static + Debug,
        F: FnMut(Entity, &A, &B),
    {
        let Some(a_component_id) = self.get_component_id::<A>() else {
            return;
        };

        let Some(b_component_id) = self.get_component_id::<B>() else {
            return;
        };

        if a_component_id == b_component_id {
            panic!("can't create component view for same component type");
        }

        let a_component_set = &self.components[a_component_id];
        let a_specific_set = a_component_set
            .as_any()
            .downcast_ref::<SparseSet<A>>()
            .unwrap();

        let b_component_set = &self.components[b_component_id];
        let b_specific_set = b_component_set
            .as_any()
            .downcast_ref::<SparseSet<B>>()
            .unwrap();

        if a_specific_set.len() < b_specific_set.len() {
            for entry in a_specific_set.iter() {
                let entity = entry.key();
                if b_specific_set.contains(entity) {
                    let a_value = entry.value();
                    let b_value = b_specific_set.get(entity).unwrap();
                    f(entity, a_value, b_value);
                }
            }
        } else {
            for entry in b_specific_set.iter() {
                let entity = entry.key();
                if a_specific_set.contains(entity) {
                    let a_value = a_specific_set.get(entity).unwrap();
                    let b_value = entry.value();
                    f(entity, a_value, b_value);
                }
            }
        };
    }

    pub fn view_two_mut<F, A, B>(&mut self, mut f: F)
    where
        A: 'static + Debug,
        B: 'static + Debug,
        F: FnMut(Entity, &mut A, &mut B),
    {
        let Some(a_component_id) = self.get_component_id::<A>() else {
            return;
        };

        let Some(b_component_id) = self.get_component_id::<B>() else {
            return;
        };

        if a_component_id == b_component_id {
            panic!("can't create component view for same component type");
        }

        let components_ptr = self.components.as_mut_ptr();

        let set = unsafe { &mut *components_ptr.add(a_component_id) };
        let a_specific_set = set.as_any_mut().downcast_mut::<SparseSet<A>>().unwrap();

        let set = unsafe { &mut *components_ptr.add(b_component_id) };
        let b_specific_set = set.as_any_mut().downcast_mut::<SparseSet<B>>().unwrap();

        if a_specific_set.len() < b_specific_set.len() {
            for entry in a_specific_set.iter_mut() {
                let entity = entry.key();
                if b_specific_set.contains(entity) {
                    let a_value = entry.value_mut();
                    let b_value = b_specific_set.get_mut(entity).unwrap();
                    f(entity, a_value, b_value);
                }
            }
        } else {
            for entry in b_specific_set.iter_mut() {
                let entity = entry.key();
                if a_specific_set.contains(entity) {
                    let a_value = a_specific_set.get_mut(entity).unwrap();
                    let b_value = entry.value_mut();
                    f(entity, a_value, b_value);
                }
            }
        };
    }

    fn get_component_id<T: 'static + Debug>(&self) -> Option<usize> {
        let type_id = TypeId::of::<T>();
        if self.component_indices.contains_key(&type_id) {
            Some(self.component_indices[&type_id])
        } else {
            None
        }
    }

    fn get_component_id_or_construct<T: 'static + Debug>(&mut self) -> usize {
        let component = self.get_component_id::<T>();
        if let Some(component_id) = component {
            return component_id;
        }

        let type_id = TypeId::of::<T>();

        let new_index = self.components.len();
        self.component_indices.insert(type_id, new_index);
        self.components.push(Box::new(SparseSet::<T>::new()));
        new_index
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    use hyper_core::handle::Handle;

    #[test]
    fn create_entities() {
        let mut registry = Registry::new();
        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        let entity_2 = registry.create_entity();
        assert_eq!(entity_2.value(), 0x00003000);
        assert_eq!(entity_2.id(), 3);
        assert_eq!(entity_2.version(), 0);
    }

    #[test]
    fn recycle_entities() {
        let mut registry = Registry::new();

        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        let entity_2 = registry.create_entity();
        assert_eq!(entity_2.value(), 0x00003000);
        assert_eq!(entity_2.id(), 3);
        assert_eq!(entity_2.version(), 0);

        registry.destroy_entity(entity_0);
        registry.destroy_entity(entity_2);

        let entity_3 = registry.create_entity();
        assert_eq!(entity_3.value(), 0x00003001);
        assert_eq!(entity_3.id(), 3);
        assert_eq!(entity_3.version(), 1);

        let entity_4 = registry.create_entity();
        assert_eq!(entity_4.value(), 0x00001001);
        assert_eq!(entity_4.id(), 1);
        assert_eq!(entity_4.version(), 1);

        let entity_5 = registry.create_entity();
        assert_eq!(entity_5.value(), 0x00004000);
        assert_eq!(entity_5.id(), 4);
        assert_eq!(entity_5.version(), 0);
    }

    #[test]
    fn add_components() {
        let mut registry = Registry::new();

        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        #[derive(Debug, PartialEq)]
        struct Position {
            x: f32,
            y: f32,
        }

        registry.add_component(entity_0, Position { x: 0.0, y: 0.0 });
        registry.add_component(entity_1, Position { x: 1.0, y: 1.0 });

        assert_eq!(
            registry.get_component(entity_0),
            Some(&Position { x: 0.0, y: 0.0 })
        );

        assert_eq!(
            registry.get_component(entity_1),
            Some(&Position { x: 1.0, y: 1.0 })
        );
    }

    #[test]
    fn remove_components() {
        let mut registry = Registry::new();

        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        #[derive(Debug, PartialEq)]
        struct Position {
            x: f32,
            y: f32,
        }

        registry.add_component(entity_0, Position { x: 0.0, y: 0.0 });
        registry.add_component(entity_1, Position { x: 1.0, y: 1.0 });

        assert_eq!(
            registry.get_component(entity_0),
            Some(&Position { x: 0.0, y: 0.0 })
        );

        assert_eq!(
            registry.get_component(entity_1),
            Some(&Position { x: 1.0, y: 1.0 })
        );

        registry.remove_component::<Position>(entity_0);
        registry.remove_component::<Position>(entity_1);

        assert_eq!(registry.get_component::<Position>(entity_0), None);
        assert_eq!(registry.get_component::<Position>(entity_1), None);
    }

    #[test]
    fn view() {
        let mut registry = Registry::new();

        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        #[derive(Debug, PartialEq)]
        struct Position {
            x: f32,
            y: f32,
        }

        #[derive(Debug, PartialEq)]
        struct Velocity {
            dx: f32,
            dy: f32,
        }

        registry.add_component(entity_0, Position { x: 0.0, y: 0.0 });
        registry.add_component(entity_1, Position { x: 1.0, y: 1.0 });
        registry.add_component(entity_1, Velocity { dx: 1.0, dy: 1.0 });

        assert_eq!(
            registry.get_component(entity_0),
            Some(&Position { x: 0.0, y: 0.0 })
        );

        assert_eq!(
            registry.get_component(entity_1),
            Some(&Position { x: 1.0, y: 1.0 })
        );

        assert_eq!(
            registry.get_component(entity_1),
            Some(&Velocity { dx: 1.0, dy: 1.0 })
        );

        let mut i = 0;
        registry.view_one(|entity: Entity, position: &Position| {
            assert_eq!(entity, Entity::from_id(i + 1));
            assert_eq!(
                *position,
                Position {
                    x: i as f32 * 1.0,
                    y: i as f32 * 1.0
                }
            );
            i += 1;
        });

        registry.view_one(|entity: Entity, velocity: &Velocity| {
            assert_eq!(entity, Entity::from_id(2));
            assert_eq!(*velocity, Velocity { dx: 1.0, dy: 1.0 });
        });

        let mut i = 0;
        registry.view_one_mut(|entity: Entity, position: &mut Position| {
            assert_eq!(entity, Entity::from_id(i + 1));
            assert_eq!(
                *position,
                Position {
                    x: i as f32 * 1.0,
                    y: i as f32 * 1.0
                }
            );
            i += 1;
        });

        registry.view_one_mut(|entity: Entity, velocity: &mut Velocity| {
            assert_eq!(entity, Entity::from_id(2));
            assert_eq!(*velocity, Velocity { dx: 1.0, dy: 1.0 });
        });

        registry.view_two(|entity: Entity, position: &Position, velocity: &Velocity| {
            assert_eq!(entity, Entity::from_id(2));
            assert_eq!(*position, Position { x: 1.0, y: 1.0 });
            assert_eq!(*velocity, Velocity { dx: 1.0, dy: 1.0 });
        });

        registry.view_two_mut(
            |entity: Entity, position: &mut Position, velocity: &mut Velocity| {
                assert_eq!(entity, Entity::from_id(2));
                assert_eq!(*position, Position { x: 1.0, y: 1.0 });
                assert_eq!(*velocity, Velocity { dx: 1.0, dy: 1.0 });
            },
        );
    }

    #[test]
    #[should_panic]
    fn view_panic() {
        let mut registry = Registry::new();

        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        #[derive(Debug, PartialEq)]
        struct Position {
            x: f32,
            y: f32,
        }

        registry.add_component(entity_0, Position { x: 0.0, y: 0.0 });
        registry.add_component(entity_1, Position { x: 1.0, y: 1.0 });

        assert_eq!(
            registry.get_component(entity_0),
            Some(&Position { x: 0.0, y: 0.0 })
        );

        assert_eq!(
            registry.get_component(entity_1),
            Some(&Position { x: 1.0, y: 1.0 })
        );

        // Should panic
        registry.view_two(|_: Entity, _: &Position, _: &Position| {});
    }

    #[test]
    #[should_panic]
    fn version_panic() {
        let mut registry = Registry::new();

        let entity_0 = registry.create_entity();
        assert_eq!(entity_0.value(), 0x00001000);
        assert_eq!(entity_0.id(), 1);
        assert_eq!(entity_0.version(), 0);

        let entity_1 = registry.create_entity();
        assert_eq!(entity_1.value(), 0x00002000);
        assert_eq!(entity_1.id(), 2);
        assert_eq!(entity_1.version(), 0);

        let entity_2 = registry.create_entity();
        assert_eq!(entity_2.value(), 0x00003000);
        assert_eq!(entity_2.id(), 3);
        assert_eq!(entity_2.version(), 0);

        registry.destroy_entity(entity_1);

        #[derive(Debug)]
        struct Position {
            x: f32,
            y: f32,
        }

        registry.add_component(entity_0, Position { x: 0.0, y: 0.0 });
        registry.add_component(entity_2, Position { x: 0.0, y: 0.0 });

        // Should panic
        registry.add_component(entity_1, Position { x: 0.0, y: 0.0 });
    }
}
