# cppevent
A C#-like event mechanism for C++.

### Thread safe multicast delegate
```cpp
cppevent::signal<void(int, int)> s;

auto token =
    s += [](int x, int y) { std::cout << x << ", " << y << std::endl; };

s(1, 2);

s -= token;
```
### Thread safe event (raise only by owning class, compile error otherwise)
```cpp
struct Publisher
{
    cppevent::event<Publisher, void(string)> SomethingHappened;
    void MakeSomethingHappen()
    {
        SomethingHappened("This just in!");
    }
};

void Subscribe(Publisher& p)
{
    auto token = 
        p.SomethingHappened += [](string news) { cout << "This happened: " << news << endl; };

    //p.SomethingHappened(); // Compilation Error. 
                             // Only the publisher can raise the event (unlike the case of cppevent::signal)
};
```

## Why use this?

Because:

- Hedaer only
- Thread Safe
- `cppevent::event` allows for in-class raise only (just like in C#)
- C#-like interface: `+=`, `-=`, `()`
