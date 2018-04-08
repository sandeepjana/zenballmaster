#if 0
#include <functional>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <vector>
#include <string>


using Text = std::vector<std::string>;

using FunctionType = std::function<void()>;

struct Foo
{
	void bar(FunctionType&& r) {}
};

class NotificationQueue
{
	std::deque<FunctionType> m_q;
	std::condition_variable m_ready;
	std::mutex	m_mutex;
	bool m_done = false;

public:
	//pop should block if no items are available
	//if an item is available, pop it atomically
	bool pop(FunctionType& f)
	{
		std::unique_lock<std::mutex> lock(m_mutex); //locks the mutex by default
		while (true == empty() && false == m_done) //loop to handle spurious awakes
			m_ready.wait(lock);
		//m_ready.wait(lock, [this]() { return false == empty() || m_done; }); //see at the end
		if (true == empty()) //&& m_done is redundant
			return false;

		f = std::move(m_q.front());
		m_q.pop_front();
		return true;
	}

	void done()
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_done = true;
		}
		m_ready.notify_all();
	}

	void push(FunctionType&& f)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_q.emplace_back(std::forward<FunctionType>(f));
			//also works fine: m_q.emplace_back(std::move(f)); //see at the end of page
		} //see note
		/*The notifying thread does not need to hold the lock on the same mutex as the one held by the waiting thread(s); in fact doing so is a pessimization, since the notified thread would immediately block again, waiting for the notifying thread to release the lock.*/
		m_ready.notify_one();
	}

	bool empty() 
	{
		return m_q.empty();
	}
};

struct StatefulFun
{
	int state = 0;

	StatefulFun() = default;

	void operator() ()
	{
		std::cout << state << '\n';
	}
	StatefulFun(const StatefulFun&) = default;
	//{
	//	std::cout << "copy ctor\n";
	//}
	StatefulFun& operator=(const StatefulFun&) {
		std::cout << "copy assignment";
	}

	StatefulFun(StatefulFun&&) = default;
	//StatefulFun(StatefulFun&&) {
	//	std::cout << "move ctor\n";
	//}
};

int main(int, char**)
{
	NotificationQueue q;

	q.push([]() {});
	auto voidfun = []() {};
	q.push(voidfun);

	auto local = 5;
	auto stuff = [&]() { std::cout << local << '\n'; };
	q.push(stuff);

	StatefulFun s;
	s.state = 10;
	q.push(s);

	q.done();

	FunctionType f;
	while (true == q.pop(f))
	{
		f();
	}
	std::cout << "done\n";

}


/* 
http://stackoverflow.com/questions/28828159/usage-of-stdforward-vs-stdmove
when not a template:
void ImageView::setImage(Image&& image){
_image = std::forward(image); //error
_image = std::move(image);//conventional
_image = std::forward<Image>(image);//unconventional
}
template <typename T> 
void ImageView::setImage(T&& image){
	_image = std::forward<T>(image);
}
*/

/*
void wait( std::unique_lock<std::mutex>& lock, Predicate wait_termination_condition ) equivalent to 
while (!wait_termination_condition()) {
wait(lock);
}
*/
#endif