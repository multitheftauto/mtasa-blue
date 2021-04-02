#### `CustomEvent` and `BuiltInEvent` (Event.h):
All event objects are interned: This way we can use plain pointer comparasion to decide if two `Event`'s are the same
##### `CustomEvent`
Shepherd (factory) class is `CustomEventShepherd`. Represents an event created by the user.
##### `BuiltInEvent` 
All built-in-events are defined in `BuiltInEvents.h`. Each object of this type has a unique `ID` assigned to it, this way we can eliminate the need of using hash maps for built-in events.

#### EventDispatcher.h
It is responsible for calling the event handlers by traversing the element tree.
It also handles `CancelEvent()`.

#### EventHandler
Basically a small wrapper around a `CLuaFunctionRef` with some additional bookkeeping stuff.

#### EventHandlerCollection
A collection of handlers attached to the same event and element.

#### EventHandlerCallDispatcher
A per-element "manager". It's main purpose is to deal with the `Event` => `EventHandlerCollection` mapping.
A side-note: As you might've seen, this class has 2 members: `m_customs` and `m_builtins`. `m_customs` should be quite obvious, but `m_builtins` might not be: It needs to be indexed with `BuiltInEvent::ID`.



