#pragma once
#include <vector>

template <typename T>
class m_Subject;

template <typename T>
class Observer {
public:
	virtual ~Observer() {}
	virtual void Update(const m_Subject<T>* subject, const T& data) = 0;
};

template <typename T>
class m_Subject {
public:
	virtual ~m_Subject() {}
	void RegisterObserver(Observer<T>* observer) { observers_.push_back(observer); }
	void RemoveObserver(Observer<T>* observer) {
		observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
	}
	virtual void Notify(const T& data) = 0;

protected:
	std::vector<Observer<T>*> observers_;
};
