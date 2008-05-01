package cn.raptium.library.patterns.Observer;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

public class ObserverTest {
	private Subject testObj;
	private testObserver testO;

	@Before
    public void setUp() throws Exception{
		testObj = new Subject();
		testO = new testObserver();
    }

	@Test
	public void testSubject() {

	}

	@Test
	public void testAttachDetach() {
		testObj.attach(testO);
		assertEquals(true, testObj.detach(testO));
		assertEquals(false, testObj.detach(testO));
	}

	@Test
	public void testNotifyObservers() {
		testObserver O1 = new testObserver();
		testObserver O2 = new testObserver();
		testObserver O3 = new testObserver();
		testObj.attach(O1);
		testObj.notifyObservers();
		assertEquals(1, O1.n);
		testObj.attach(O2);
		testObj.notifyObservers();
		assertEquals(2, O1.n);
		assertEquals(1, O2.n);
		testObj.attach(O3);
		testObj.notifyObservers();
		assertEquals(3, O1.n);
		assertEquals(2, O2.n);
		assertEquals(1, O3.n);
		testObj.detach(O2);
		testObj.notifyObservers();
		assertEquals(4, O1.n);
		assertEquals(2, O2.n);
		assertEquals(2, O3.n);
	}
}

class testObserver extends Observer{
	public int n = 0;
	@Override
	public void update() {
		n++;
	}
}
