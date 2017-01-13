package be.ac.ulb.infof307.g01.socialGraph.model;

import java.awt.Color;

public class Node implements INode {
	
	private static int SCALE=10;
	private String name="user";
	private int x=0;
	private int y=0;
	private static int r=2*SCALE+5;
	private Color color=Color.blue;
	
	public Node(String name) {
		this.setName(name);
	}
	
	public Node(int x, int y, String name) {
		this(name);
		this.setPosition(x,y);
	}
	
	@Override
	public void setPosition(int x, int y) {
		this.x = x;
		this.y = y;
	}
	
	@Override
	public void setName(String name) {
		this.name=name;
	}
	
	@Override
	public void setColor(Color color) {
		this.color=color;
	}
	
	@Override
	public void setScale(int scale) {
		if(scale<=0)
			scale=1;
		Node.SCALE=scale;
	}
	
	@Override
	public int getX() {
		return this.x;
	}
	
	@Override
	public int getY() {
		return this.y;
	}
	
	@Override
	public int getR() {
		return Node.r;
	}
	
	public static int getRadius() {
		return Node.r;
	}
	
	@Override
	public String getName() {
		return this.name;
	}
	
	@Override
	public Color getColor() {
		return this.color;
	}
	
	@Override
	public int getScale() {
		return Node.SCALE;
	}
}