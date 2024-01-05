#include <iostream>
#include <string>
#include <assert.h>

class Task
{
  friend class Queue;
  friend class Stack;

private:
  static const int _max_process_duration = 47;

  std::string _name;
  double _time;
  Task *_next {};

public:
  const std::string & name() { return _name; }
  double time() { return _time; }
  Task * next() { return _next; }

  void set_name(const std::string &name) { _name = name; }
  void set_time(double time) { _time = time; }

public:
  Task(std::string name, double time) : _name(name), _time(time) {}
  Task(std::string name) : _name(name), _time(rand() % _max_process_duration) {}

  Task(const Task &t) = default;
  Task & operator=(const Task &t) = default;

  static Task generate_random()
  {
    static int number = 0;
    return Task(std::string("Task #") + std::to_string(number++));
  }

public:
  void print()
  {
    std::cout << _name << ": " << _time << "s left\n";
  }
};

class List
{
public:
  virtual int size() = 0;
  virtual Task * peek() = 0;
  virtual Task pop() = 0;
  virtual void push(Task task) = 0;
};

class Queue : public List
{
  Task *_head {}, *_tail {};
  int _size {};

public:
  Queue() {};

  Task * peek() override 
  {
    if (_head == nullptr)
      throw std::exception("top of empty queue");
    return _head; 
  }

  Task pop() override 
  {
    if (_head == nullptr)
      throw std::exception("pop from empty queue");

    _size--;
    Task save = *_head;
    if (_tail == _head)
      _tail = nullptr;
    delete _head;
    _head = save._next;
    save._next = nullptr; // ??
    return save;
  }

  void push(Task task) override 
  {
    _size++;
    Task *ptr = new Task(task);
    if (_tail == nullptr)
      _tail = _head = ptr;
    else
    {
      _tail->_next = ptr;
      _tail = ptr;
    }
  }

  int size() override { return _size; }

  ~Queue()
  {
    while (_head != nullptr)
    {
      Task *save = _head;
      _head = _head->_next;
      delete save;
    }
  }
};

class Stack : public List
{
  Task *_top {};
  int _size {};

public:
  Stack() {}

  Task * peek() override 
  {
    if (_top == nullptr)
      throw std::exception("top of empty stack");
    return _top; 
  }

  Task pop() override 
  {
    if (_top == nullptr)
      throw std::exception("pop from empty stack");

    _size--;
    Task save = *_top;
    delete _top;
    _top = save._next;
    return save;
  }

  void push(Task task) override 
  {
    _size++;
    Task *ptr = new Task(task);
    ptr->_next = _top;
    _top = ptr;
  }

  int size() override { return _size; }

  ~Stack()
  {
    while (_top != nullptr)
    {
      Task *save = _top;
      _top = _top->_next;
      delete save;
    }
  }
};

template<typename ListType>
class TaskManager
{
  ListType *_main_list;
  List *_list;
  double time = 0;

public:
  TaskManager()
  {
    _main_list = new ListType();
    _list = _main_list;
  }
  ~TaskManager() { delete _main_list; }

  void add_task(Task task) { _list->push(task); }
  void delete_last_task() { _list->pop(); }

  void update()
  {
    double delta_time = clock() / (double)CLOCKS_PER_SEC - time;
    time = clock() / (double)CLOCKS_PER_SEC;
    while (delta_time > 0 && _list->size() != 0)
    {
      if (delta_time < _list->peek()->time())
      {
        _list->peek()->set_time(_list->peek()->time() - delta_time);  
        break;
      }
      else
      {
        delta_time -= _list->peek()->time();
        std::cout << _list->pop().name() << " completed\n\n";
      }
    }
  }

  void print_tasks()
  {
    if (_list->size() == 0)
    {
      std::cout << "There isnt any task\n";
      return;
    }
    std::cout << "There are " << _list->size() << " tasks to do\n";
    for (Task *t = _list->peek(); t != nullptr; t = t->next())
      t->print();
    std::cout << '\n';
  }
};

int main()
{
  auto t = TaskManager<Queue>();
  t.add_task(Task("a", 10));
  t.add_task(Task("b", 3));
  t.add_task(Task::generate_random());
  t.add_task(Task::generate_random());
  
  t.print_tasks();
  double time = 0, last_tick = 0, delta = 1;
  for (;;)
  {
    time = clock() / (double)CLOCKS_PER_SEC;
    if (time - last_tick > delta)
    {
      last_tick = time;
      t.update();
      t.print_tasks();
    }
  }
  t.print_tasks();
}
 