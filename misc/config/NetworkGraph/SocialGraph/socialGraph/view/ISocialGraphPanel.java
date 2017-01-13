package be.ac.ulb.infof307.g01.socialGraph.view;

public interface ISocialGraphPanel {

	/**
	 * Method to close the social graph.
	 */
	void closeSocialGraph();
	
	/**
	 * Method to export the social graph in a png format.
	 * @param format
	 * @param filePath
	 */
	void export(String format, String filePath);
}
