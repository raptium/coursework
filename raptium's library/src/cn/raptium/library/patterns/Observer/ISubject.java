package cn.raptium.library.patterns.Observer;

public interface ISubject {
	void attach(IObserver observer);
	boolean detach(IObserver observer);
	public void clear();
	void notifyObservers();
}
