package cn.raptium.library.patterns.Visitor;

public interface IElement {
	void accept(IVisitor vistor);
}
