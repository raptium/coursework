package cn.raptium.library.patterns.Observer;
import java.util.ArrayList;

public class Subject implements ISubject {
	private ArrayList<IObserver> m_observers;

	public Subject(){
		m_observers = new ArrayList<IObserver>();
	}

	public void attach(IObserver observer) {
		m_observers.add(observer);
	}

	public boolean detach(IObserver observer) {
		return m_observers.remove(observer);
	}

	public void clear(){
		m_observers.clear();
	}

	public void notifyObservers() {
		for(IObserver observer: m_observers){
			observer.update();
		}
	}
}
