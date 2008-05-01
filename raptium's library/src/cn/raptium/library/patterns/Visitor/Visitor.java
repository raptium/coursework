package cn.raptium.library.patterns.Visitor;

public class Visitor implements IVisitor {

	public void visit(IElement element) {
		element.accept(this);
	}

}
