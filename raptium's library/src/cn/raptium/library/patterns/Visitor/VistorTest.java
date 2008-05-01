package cn.raptium.library.patterns.Visitor;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;


public class VistorTest {

	private Boy boy;
	private Girl girl;
	@Before
	public void setUp() throws Exception{
		boy = new Boy();
		girl = new Girl();
    }

	@Test
	public void doubledispatchTest(){
		assertEquals("Boy meets boy.", boy.accept(boy));
		assertEquals("Girl meets girl.", girl.accept(girl));
		assertEquals("Girl meets boy.", girl.accept(boy));
		assertEquals("Boy meets girl.", boy.accept(girl));
	}
}


class Person extends Visitor implements IElement{
	public void accept(IVisitor vistor) {
		vistor.visit(this);
	}
}

class Boy extends Person{
	public String accept(Boy p){
		return "Boy meets boy.";
	}
	public String accept(Girl p){
		return "Boy meets girl.";
	}
}

class Girl extends Person{
	public String accept(Boy p){
		return "Girl meets boy.";
	}
	public String accept(Girl p){
		return "Girl meets girl.";
	}
}
