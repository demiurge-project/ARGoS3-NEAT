package be.ac.ulb.infof307.g01.socialGraph.model;

import java.util.ArrayList;
import java.util.Random;

public class Graph implements IGraph {
	
	private ArrayList<INode> listNodes = null;
	private ArrayList<IEdge> listEdges = null;

	public Graph(ArrayList<INode> ln, ArrayList<IEdge> le) {
		setListNodeAndListEdge(ln, le);
	}
	
	public Graph() {
		this(null, null);
	}
	
	public void setListNodeAndListEdge(ArrayList<INode> ln, ArrayList<IEdge> le) {
		if(ln==null)
			this.listNodes=new ArrayList<INode>();
		else
			this.listNodes=ln;
		if(le==null)
			this.listEdges=new ArrayList<IEdge>();
		else
			this.listEdges=le;
	}

	//------------------- NODES -------------------//
	@Override
	public void addNode(INode n) {
		if(contains(n)==-1)
			this.listNodes.add(n);
	}

	@Override
	public void addNode(ArrayList<INode> ln) {
		for(INode n : ln) {
			if(contains(n)==-1)
				this.listNodes.add(n);
		}
	}

	@Override
	public void removeNode(INode n) {
		int i=contains(n);
		if(i>-1)
			this.listNodes.remove(i);
	}

	@Override
	public void removeNode(int index) {
		if(index>-1 && index<this.listNodes.size())
			this.listNodes.remove(index);
	}

	//------------------- EDGES -------------------//
	@Override
	public void addEdge(IEdge e) {
		int i=contains(e);
		if(i==-1)
			this.listEdges.add(e);
		else
			this.listEdges.get(i).setWeight(e.getWeight());
	}

	@Override
	public void addEdge(ArrayList<IEdge> le) {
		int i=0;
		for(IEdge e : le) {
			i=contains(e);
			if(i==-1)
				this.listEdges.add(e);
			else
				this.listEdges.get(i).setWeight(e.getWeight());
		}
	}

	@Override
	public void removeEdge(IEdge e) {
		int i=contains(e);
		if(i>-1)
			this.listEdges.remove(i);
	}

	@Override
	public void removeEdge(int index) {
		if(index>-1 && index<this.listEdges.size())
			this.listEdges.remove(index);
	}

	@Override
	public void removeEdge(INode n) {
		ArrayList<IEdge> le = new ArrayList<IEdge>();
		for(IEdge e : this.listEdges) {
			if(e.contains(n))
				le.add(e);
		}
		this.listEdges.removeAll(le);
	}

	//------------------- GETTERS -------------------//
	@Override
	public INode getNode(int index) {
		if(index<0)
			return this.listNodes.get(0);
		if(index>this.listNodes.size())
			return this.listNodes.get(this.listNodes.size()-1);
		return this.listNodes.get(index);
	}

	@Override
	public IEdge getEdge(int index) {
		if(index<0)
			return this.listEdges.get(0);
		if(index>this.listEdges.size())
			return this.listEdges.get(this.listEdges.size()-1);
		return this.listEdges.get(index);
	}

	@Override
	public ArrayList<INode> getListNodes() {
		return this.listNodes;
	}

	@Override
	public ArrayList<IEdge> getListEdges() {
		return this.listEdges;
	}

	@Override
	public int getListNodesSize() {
		return this.listNodes.size();
	}

	@Override
	public int getListEdgesSize() {
		return this.listEdges.size();
	}

	//------------------- OTHER METHODS -------------------//
	@Override
	public int contains(INode node) {
		int i=0;
		for(INode n : this.listNodes) {
			if(n.getName().equals(node.getName()))
				return i;
			i++;
		}
		return -1;
	}

	@Override
	public int contains(IEdge edge) {
		int i=0;
		for(IEdge e : this.listEdges) {
			if( (e.getNode1().getName().equals(edge.getNode1().getName()) && 
					e.getNode2().getName().equals(edge.getNode2().getName())) ||
					(e.getNode1().getName().equals(edge.getNode2().getName()) &&
							e.getNode2().getName().equals(edge.getNode1().getName()))) {
				return i;
			}
			i++;
		}
		return -1;
	}

	@Override
	public void setPositionToAllNodes(int width, int height) {
		Random randomGenerator = new Random();
		int x=0, y=0, pos=0, attempt=0;
		int size=this.listNodes.size();
		boolean good=true;
		for(int i=0; i<size; i++) {
			x = randomGenerator.nextInt(width-Node.getRadius()*2)+Node.getRadius();
			y = randomGenerator.nextInt(height-Node.getRadius()*2)+Node.getRadius();
			
			//check if there is already a node a this place
			for(int j=0; j<size && good==true; j++) {
				if(x>=(listNodes.get(i).getX()) && x<=(listNodes.get(i).getX()+2*listNodes.get(i).getR())
						&& y>=(listNodes.get(i).getY()) && y<=(listNodes.get(i).getY()+2*listNodes.get(i).getR())) {
					good=false;
					attempt++;
				}
			}
			
			if(attempt>=200) {
				good=true;
				attempt=200;
			}
			
			if(good) {
				this.listNodes.get(i).setPosition(x, y);
				pos=i;
			} else {
				i=pos-1;
			}
			good=true;
		}
	}
}
