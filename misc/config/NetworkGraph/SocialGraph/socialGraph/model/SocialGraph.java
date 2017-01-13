package be.ac.ulb.infof307.g01.socialGraph.model;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import be.ac.ulb.infof307.g01.model.DAO.AbstractDAOFactory;
import be.ac.ulb.infof307.g01.model.DAO.DAO;
import be.ac.ulb.infof307.g01.model.DAO.Request.RequestAuthorsRelationBySecondAuthorID;
import be.ac.ulb.infof307.g01.model.DAO.Request.RequestAuthorsRelationByUserID;
import be.ac.ulb.infof307.g01.model.Fields.IAuthor;
import be.ac.ulb.infof307.g01.model.Fields.IUser;

public class SocialGraph {

	private IUser user;
	private int socialDepth;
	
	public SocialGraph(IUser user, int n) {
		this.user = user;
		this.socialDepth = n;
		//System.out.println("user: "+this.user.getName()+", social depth: "+this.socialDepth);
	}

	public IGraph getGraph() {
		IGraph graph = new Graph();
		
		//DBC con = DBC.getDBC();
		
		INode userNode = new Node("YOU");
		graph.addNode(userNode);
		
		//Fetch the coauthors
		int userAuthorId = this.user.getAuthorID();
		AbstractDAOFactory adf = AbstractDAOFactory.getFactory(AbstractDAOFactory.SQLITE_FACTORY);
		DAO<IAuthor> autDAO = adf.getDAOAuthor();
		List<IAuthor> firstAuthors = autDAO.find(new RequestAuthorsRelationByUserID(this.user.getID()));
		Map<String, Integer> paperCount = countPapers(firstAuthors);
		
		//Build the graph
		for(String authorName : paperCount.keySet()) {
			INode node = new Node(authorName);
			IEdge edge = new Edge(userNode, node, paperCount.get(authorName));
			graph.addNode(node);
			graph.addEdge(edge);
			//System.out.println("authorName: "+authorName+", count: "+paperCount.get(authorName));
			//Build for the next depths as well.
			getAuthorsRelatedTo(findAuthorID(authorName, firstAuthors), node, userAuthorId, graph, this.socialDepth-1, autDAO);
		}
		
		return graph;
	}
	
	/**
	 * Complete the social graph in a recursive way.
	 * @param authorId
	 * @param authorNode
	 * @param userAuthorId
	 * @param graph
	 * @param depth
	 * @param autDAO
	 * 		DAO for the authors.
	 */
	private void getAuthorsRelatedTo(int authorId, INode authorNode, int userAuthorId, IGraph graph, int depth, DAO<IAuthor> autDAO){
		if (depth >= 1){
			//System.out.println("Second.");
			List<IAuthor> secondAuthors = autDAO.find(new RequestAuthorsRelationBySecondAuthorID(authorId, userAuthorId));
			if(secondAuthors.isEmpty()) {
				System.out.println("isEmpty()");
			}

			Map<String, Integer> paperCount = countPapers(secondAuthors);
			//Build the graph
			for(String authorName : paperCount.keySet()) {
				System.out.println("in loop");
				INode node = new Node(authorName);
				IEdge edge = new Edge(authorNode, node, paperCount.get(authorName));
				graph.addNode(node);
				graph.addEdge(edge);
				//Build for the next depths as well.
				getAuthorsRelatedTo(findAuthorID(authorName, secondAuthors), node, userAuthorId, graph, depth-1, autDAO);
			}
			
			
//			try {
//				while (rs.next()){
//					INode node = new Node(rs.getString("Author_name"));
//					IEdge edge = new Edge(authorNode, node, rs.getInt("CountOfPaper"));
//					graph.addNode(node);
//					graph.addEdge(edge);
//					
//					getAuthorsRelatedTo(rs.getInt("CoAuthor_ID"), node, userAuthorId, graph, depth-1);
//				}
//			} catch (SQLException e) {
//				ErrorLog.logToFile(e.getMessage(), e.getStackTrace());
//				e.printStackTrace();
//			}
		}
	}
	
	/**
	 * Counts the duplicates in the argument list to deduce
	 * the number of publications.
	 * @param authors
	 * 		List of authors.
	 * @return
	 * 		<Author name, count>
	 */
	private Map<String, Integer> countPapers(List<IAuthor> authors) {
		System.out.println("number: "+authors.size());
		Map<String, Integer> paperCount = new HashMap<String, Integer>();
		for(IAuthor author : authors) {
			String name = author.getName();
			if(paperCount.containsKey(name)) {
				int count = paperCount.get(name);
				count++;
				paperCount.put(name, count);
			}
			else {
				paperCount.put(name, 1);
			}
		}
		return paperCount;
	}
	
	/**
	 * Search the id of the author whose name is given as argument,
	 * inside the list also given as argument.
	 * @param name
	 * 		Name to search for.
	 * @param authors
	 * 		List to search in.
	 * @return
	 * 		ID of the searched author.
	 */
	private int findAuthorID(String name, List<IAuthor> authors) {
		boolean found = false;
		int i = 0;
		int id = -1;
		while(!found && i<authors.size()) {
			IAuthor author = authors.get(i);
			if(name.equals(author.getName())) {
				found = true;
				id = author.getID();
			}
			else {
				i++;
			}
		}
		return id;
	}

}
