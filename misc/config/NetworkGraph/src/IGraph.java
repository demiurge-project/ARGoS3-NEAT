import java.util.ArrayList;

/**
 * Graph Interface.
 * @author Brian Delhaisse
 */
public interface IGraph {

	/**
	 * Adds a new node to the graph.
	 * If the node has already been defined (i.e. it has the same name with an other node which is 
	 * already in the graph), then it will not be added to the graph.
	 * @param node the node.
	 */
	void addNode(INode node);

	/**
	 * Adds a new list of nodes to the graph.
	 * If some nodes have already been defined, they won't be added to the graph.
	 * @param listNode the list of nodes.
	 */
	void addNode(ArrayList<INode> listNode);

	/**
	 * Removes a node from the graph.
	 * @param node the node in question.
	 */
	void removeNode(INode node);

	/**
	 * Removes the ith node from the graph.
	 * @param i index.
	 */
	void removeNode(int i);

	/**
	 * Adds a new edge (connecting 2 nodes) to the graph.
	 * If the edge has already been defined, it will just change the weight of the edge.
	 * @param edge the new edge.
	 */
	void addEdge(IEdge edge);

	/**
	 * Adds a new list of edges to the graph.
	 * If some edges have already been defined, it will just change the weight of those edges.
	 * @param listEdge the list of edges.
	 */
	void addEdge(ArrayList<IEdge> listEdge);

	/**
	 * Removes an edge from the graph.
	 * @param edge the edge in question.
	 */
	void removeEdge(IEdge edge);

	/**
	 * Removes the ith edge from the graph.
	 * @param i index.
	 */
	void removeEdge(int i);

	/**
	 * Removes all edges which have a common node at one extremity.
	 * @param node the common node.
	 */
	void removeEdge(INode node);
	
	/**
	 * Gets the ith node.
	 * @param i index.
	 * @return the ith node.
	 */
	INode getNode(int i);
	
	INode getNodeFromId(String id);
	/**
	 * Gets the ith edge.
	 * @param i index.
	 * @return the ith edge.
	 */
	IEdge getEdge(int i);

	/**
	 * Gets the list of all nodes.
	 * @return the list of all nodes.
	 */
	ArrayList<INode> getListNodes();

	/**
	 * Gets the list of all edges.
	 * @return the list of all edges.
	 */
	ArrayList<IEdge> getListEdges();

	/**
	 * Gets the size of the list of all nodes.
	 * @return size of the list of all nodes.
	 */
	int getListNodesSize();

	/**
	 * Gets the size of the list of all edges.
	 * @return size of the list of all edges.
	 */
	int getListEdgesSize();

	/**
	 * Checks if the graph contains a specific node.
	 * @param node the specific node in question.
	 * @return integer. Returns an integer>0 if the graph contains the node. 
	 * In this case, the integer is the index of the node in the graph.
	 * Returns -1 if the graph doesn't contain the node.
	 */
	int contains(INode node);

	/**
	 * Checks if the graph contains a specific edge connecting 2 nodes.
	 * @param edge the specific edge in question.
	 * @return integer. Returns an integer>0 if the graph contains the edge. 
	 * In this case, the integer is the index of the edge in the graph.
	 * Returns -1 if the graph doesn't contain the edge.
	 */
	int contains(IEdge edge);

	/**
	 * Sets the position of all nodes in the graph.
	 * @param width width of the graph.
	 * @param height height of the graph.
	 */
	void setPositionToAllNodes(int width, int height);
	public String writeHeader(String id);
	public String writeTrait();
	public void setTypeFromEdgesAndBias(INode n);
	public String getNodeTypeAndGeneticNodeType(INode n);
	public String writeNodes();
	public String writeGenes();
	public String writeEndLine(String id);
	
}
