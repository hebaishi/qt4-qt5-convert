# qt4-qt5-convert
Tool for automatic conversion of Qt4 signal/slot connections to Qt5

This aim here is to create a tool that will convert Qt4 signal/slot connections to the new Qt5 syntax using clang's AST.

The end-goal is convert this:

```cpp
connect(
    sender, SIGNAL( valueChanged( QString, QString ) ),
    receiver, SLOT( updateValue( QString ) )
);

```
into this:
```cpp
connect(
    sender, &Sender::valueChanged,
    receiver, &Receiver::updateValue
);
```
