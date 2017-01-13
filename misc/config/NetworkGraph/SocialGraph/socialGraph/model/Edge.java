package be.ac.ulb.infof307.g01.socialGraph.model;

public class Edge implements IEdge {

	private INode node1=null;
	private INode node2=null;
	private int weight=1;
	
	public Edge(INode n1, INode n2) {
		this.node1=n1;
		this.node2=n2;
	}
	
	public Edge(INode n1, INode n2, int w) {
		this(n1, n2);
		setWeight(w);
	}
	
	@Override
	public void setWeight(int w) {
		if(w<=0) 
			this.weight=1;
		else
			this.weight=w;
	}
	
	@Override
	public INode getNode1() {
		return this.node1;
	}
	
	@Override
	public INode getNode2() {
		return this.node2;
	}
	
	@Override
	public int getWeight() {
		return this.weight;
	}
	
	@Override
	public boolean contains(INode n) {
		if(this.node1.getName().equals(n.getName()) || this.node2.getName().equals(n.getName())) 
			return true;
		return false;
	}
}