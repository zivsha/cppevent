#include "cppevent/cppevent.hpp"
#include <iostream>
#include <string>
#include <thread>

#include "timer.hpp"
#include "observable_queue.hpp"

using namespace cppevent;
using namespace std::chrono;


//TODO: show how to create a token generator
// TODO   std::cout << "Unregister with a token from another event will be ignored" << std::endl;
// queue.OnChanged -= timer_token;
int main()
{
    {

        std::cout << "Creating a Timer instance with an \"Elapsed\" event, that is trigged when time is up\n";
        cppevent_demo::Timer timer(1000ms);

        std::cout << "Register to \"Elased\" event and store the token\n";
        auto timer_token =
            timer.Elapsed += []() { std::cout << "Elapsed\n"; };

        std::cout << "Start the timer\n";
        timer.Start();

        std::cout << "Sleep for 1.2 seconds\n";
        std::this_thread::sleep_for(1200ms);

        std::cout << "Removing the registration using the stored token\n";
        timer.Elapsed -= timer_token;

        std::cout << "It is a compilation error to try to raise the \"Elapsed\" event from outside of the publisher's class\n";
        //t.Elapsed(); 
        std::cout << "t.Elapsed(); // operator() is inaccessible due to its protection level\n";

    }


    {

        std::cout << "Creating an observable queue instance with an \"OnChanged\" event, that is trigged when the queue is changed\n";
        cppevent_demo::observable_queue<int> queue;

        std::cout << "Register to \"OnChanged\" event and store the token\n";
        auto queue_token =
            queue.OnChanged += [](std::size_t new_size) { std::cout << "New queue size is: " << new_size << std::endl; };

        std::cout << "Filling the queue\n";
        for (int i = 0; i < 3; i++)
        {
            std::cout << "Adding to queue" << std::endl;
            queue.put(i);
        }

        std::cout << "Emptying the queue\n";
        while ([&queue] { std::cout << "Removing item from the queue" << std::endl;
                          int out;
                          return queue.take(out);
                        }()
              );

        std::cout << "Removing the registration using the stored token\n";
        queue.OnChanged -= queue_token;
    }


    std::cout << "Signal usage examples\n";
    signal<void(int)> s0;
    signal<void(const std::string&)> s1;

}