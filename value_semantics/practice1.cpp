#if 0
#include <iosfwd>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>

template<typename T>
void draw(std::ostream& os, const T& data)
{
	os << data << std::endl;
}

class object_t {
public:
	template<typename T>
	object_t(const T& x) : object_(std::make_shared<model_t<T>>(x)) {}

	object_t(const object_t& obj) : object_(std::move(obj.object_)) {}

	object_t& operator=(object_t obj) {
		object_ = std::move(obj.object_);
	}

	friend void draw(std::ostream& os, const object_t& obj)
	{
		obj.object_->draw_(os);
	}

private:
	struct concept_t {
		virtual void draw_(std::ostream& os) const = 0;
	};

	template<typename T>
	struct model_t : concept_t { 

		model_t(const T& data) : data_(data) {}
		model_t(T&& data): data_(std::move(data)) {}

		virtual void draw_(std::ostream& os) const
		{
			draw(os, data_);
		}

		T data_;
	};

	std::shared_ptr<const concept_t> object_;
};

typedef std::vector<object_t> doc_t;
void draw(std::ostream& os, const doc_t& doc)
{
	for(auto it = doc.begin(); it != doc.end(); ++it)
		draw(os, *it);
}

int main(int , char** )
{
	doc_t doc;
	doc.reserve(3);
	doc.push_back(0);
	doc.emplace_back(1);
	doc.emplace_back(doc);

	draw(std::cout, doc);
}

#endif