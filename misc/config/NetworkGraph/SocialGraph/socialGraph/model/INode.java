package be.ac.ulb.infof307.g01.socialGraph.model;

import java.awt.Color;

public interface INode {
	//maybe the scale and the radius should be defined in the view.

	/**
	 * Sets the position (x,y) of the node.
	 * @param x position x.
	 * @param y position y.
	 */
	void setPosition(int x, int y);
	
	/**
	 * Sets the name of the node.
	 * @param name name of the node.
	 */
	void setName(String name);
	
	/**
	 * Sets the color of the node.
	 * @param color color of the node.
	 */
	void setColor(Color color);
	
	/**
	 * Sets the scale of all nodes.
	 * @param scale scale of all nodes.
	 */
	void setScale(int scale);
	
	/**
	 * Gets the position x of the node.
	 * @return position x
	 */
	int getX();
	
	/**
	 * Gets the position y of the node.
	 * @return position y
	 */
	int getY();
	
	/**
	 * Gets the radius of the node.
	 * @return position y
	 */
	int getR();
	
	/**
	 * Gets the name of the node.
	 * @return name
	 */
	String getName();
	
	/**
	 * Gets the color of the node.
	 * @return color
	 */
	Color getColor();

	/**
	 * Gets the scale of the node.
	 * @return color
	 */
	int getScale();
}
