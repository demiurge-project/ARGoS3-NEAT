
/**
 * Edge Class.
 * @author Brian Delhaisse
 */
public class Edge implements IEdge {
	
	private INode node1=null;
	private INode node2=null;
	private double weight = 0.0;
	private String historicalNumber = "0";
	
	public Edge(INode n1, INode n2) {
		this.node1=n1;
		this.node2=n2;
	}
	
	public Edge(INode n1, INode n2, int w) {
		this(n1, n2);
		setWeight(w);
	}
	
	public Edge(INode n1, INode n2, double weight, String histNb) {
		this.node1=n1;
		this.node2=n2;
		this.weight = weight;
		this.historicalNumber = histNb;
	}
	
	@Override
	public void setWeight(double w) {
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
	public double getWeight() {
		return this.weight;
	}
	
	@Override
	public boolean contains(INode n) {
		if(this.node1.getName().equals(n.getName()) || this.node2.getName().equals(n.getName())) 
			return true;
		return false;
	}
	
	// 0 : the node is not connect to another with this edge
	// 1 : this edge goes from the node to another node
	// 2 : this edge goes from another node to the node
	@Override
	public int isSourceOrDestination(INode n) {
		if(this.node1.getName().equals(n.getName()))
			return 1;
		else if (this.node2.getName().equals(n.getName()))
			return 2;
		else
			return 0;
	}
	
	@Override
	public void setHistoricalNumber(int nb) {
		historicalNumber = ""+nb;
	}
	@Override
	public String getHistoricalNumber() {
		return historicalNumber;
	}
}
