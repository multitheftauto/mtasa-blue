#### `CustomEvent` and `BuiltInEvent` (Event.h):
- All event objects are interned: This way we can use plain pointer comparasion to decide if two `Event`'s are the same
- **`CustomEvent`**: Shepherd (factory) class is `CustomEventShepherd`. This class holds an event created by the user.
- **`BuiltInEvent`**: All built-in-events are defined in `BuiltInEvents.h`. This class has an `ID` assigned to it, this way we can eliminate the need of using hash maps.



### How does all this work?



