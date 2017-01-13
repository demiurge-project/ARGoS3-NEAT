package be.ac.ulb.infof307.g01.socialGraph.model;


public interface IEdge {

	/**
	 * Sets the weight/cost of the edge.
	 * @param weight weight/cost of the edge.
	 */
	public void setWeight(int weight);
	
	/**
	 * Gets the node which is at the end of one side of the edge.
	 * @return node1 the first node.
	 */
	public INode getNode1();
	
	/**
	 * Gets the node which is at the end of the other side of the edge.
	 * @return node2 the 2nd node.
	 */
	public INode getNode2();
	
	/**
	 * Gets the weight/cost of the edge.
	 * @return weight weight/cost of the edge.
	 */
	public int getWeight();
	
	/**
	 * Checks if the node is at the end of one side of the edge. Comparison between names.
	 * @param node node to check.
	 * @return true/false 
	 */
	public boolean contains(INode node);
}
