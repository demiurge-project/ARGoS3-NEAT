import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.FileDialog;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.RadialGradientPaint;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.lang.reflect.Field;
import java.util.ArrayList;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;

/**
 * GraphPanel Class.
 * @author Brian Delhaisse
 */
@SuppressWarnings("serial")
public class GraphPanel extends JPanel implements MouseListener, MouseMotionListener, ActionListener {

	private IGraph graph=null;
	private int listNodeSize = 0;
	private int listEdgeSize = 0;

	private INode nodeDragged=null;
	private INode selectedNode[] = new INode[2];
	
	private ArrayList<INode> selectedSourceNodes = new ArrayList<INode>();
	private ArrayList<INode> selectedDestinationNodes = new ArrayList<INode>();

	private boolean repaint=false;
	private boolean repaintAll=false;
	private int mouseX=0;
	private int mouseY=0;
	private String[] colorTable={"red","blue","magenta","orange","gray","pink","green"};
	private JPopupMenu popUpMenu=null;
	private ArrayList<JMenuItem> menuItemList = new ArrayList<JMenuItem>();
	private int number=0;
	private int historicalGlobalNumber = 1;
	private boolean firstTime=true;
	
	public GraphPanel(IGraph g) {
		createJPopUpMenu();
		this.addMouseMotionListener(this);
		this.addMouseListener(this);
		setGraph(g);
	}

	public IGraph getGraph() {
		return this.graph;
	}
	
	public void setGraph(IGraph g) {
		if(g!=null) {
			this.graph=g;
			this.listNodeSize = graph.getListNodesSize();
			this.listEdgeSize = graph.getListEdgesSize();
		} else {
			this.graph = new Graph();
		}
	}

	private void createJPopUpMenu() {
		this.popUpMenu = new JPopupMenu();
		
		JMenuItem menuItem = new JMenuItem("add");
		menuItem.addActionListener(this);
		popUpMenu.add(menuItem);
		
		menuItem = new JMenuItem("remove");
		menuItem.addActionListener(this);
		popUpMenu.add(menuItem);
		
		JMenu menu = new JMenu("bias neuron");
		menuItemList.add(menu);
		
		menuItem = new JMenuItem("yes");
		menuItemList.add(menuItem);
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		menuItem = new JMenuItem("no");
		menuItemList.add(menuItem);
		menuItem.addActionListener(this);
		menu.add(menuItem);
		
		popUpMenu.add(menu);
		
		menu = new JMenu("Color");
		menuItemList.add(menu);
		
		for(String s : colorTable) {
			menuItem=new JMenuItem(s);
			menuItem.addActionListener(this);
			menu.add(menuItem);
		}
		popUpMenu.add(menu);
		
		popUpMenu.addSeparator();
		
		menuItem = new JMenuItem("link");
		menuItem.addActionListener(this);
		popUpMenu.add(menuItem);
		
		menuItem = new JMenuItem("set weight");
		menuItemList.add(menuItem);
		menuItem.addActionListener(this);
		popUpMenu.add(menuItem);
		
		popUpMenu.addSeparator();
		
		menuItem = new JMenuItem("export png");
		menuItem.addActionListener(this);
		popUpMenu.add(menuItem);
	}

	public void paintComponent(Graphics g) {
		if(firstTime) {
			this.graph.setPositionToAllNodes(this.getWidth(), this.getHeight());
			firstTime=false;
		}
		Graphics2D g2 = (Graphics2D) g;
		g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

		//Background //default value RGB=(237,237,237) when a frame is created.
		g2.setColor(Color.white);
		g2.fillRect(0, 0, this.getWidth(), this.getHeight());

		//Nodes and edges defined in the graph	
		for(int i=0; i<listEdgeSize; i++ ) {
			g2.setColor(Color.black);
			g2.setStroke(new BasicStroke((float) (2.0)));
			
			double x1 = graph.getEdge(i).getNode1().getX()+graph.getEdge(i).getNode1().getR();
			double y1 = graph.getEdge(i).getNode1().getY()+graph.getEdge(i).getNode1().getR();
			double x2 = graph.getEdge(i).getNode2().getX()+graph.getEdge(i).getNode2().getR();
			double y2 = graph.getEdge(i).getNode2().getY()+graph.getEdge(i).getNode2().getR();
			
			double alpha = Math.atan((y2-y1)/(x2-x1));
			double r = graph.getEdge(i).getNode1().getR();
			if(x2-x1<0)
				r = -r;
			
			double dist = Math.pow(Math.sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1))/10,2.0/3.0);

			if(dist > 7)
				dist = 7;
			
			double xb = x1 + (dist/(dist+1.0))*(x2-x1-r*Math.cos(alpha));
			double yb = y1 + (dist/(dist+1.0))*(y2-y1-r*Math.sin(alpha));
			
			double xr = xb + dist*Math.sin(alpha);
			double yr = yb - dist*Math.cos(alpha);
			
			double xl = xb - dist*Math.sin(alpha);
			double yl = yb + dist*Math.cos(alpha);
			
			g2.draw(new Line2D.Double(x1,y1,xb,yb));
			
			g2.draw(new Line2D.Double(xl,yl,xr,yr));
			g2.draw(new Line2D.Double(xl,yl,x2-(6.0/5.0)*r*Math.cos(alpha),y2-(6.0/5.0)*r*Math.sin(alpha)));
			g2.draw(new Line2D.Double(xr,yr,x2-(6.0/5.0)*r*Math.cos(alpha),y2-(6.0/5.0)*r*Math.sin(alpha)));
			
			
			int[] x = new int[3];
			x[0] = (int) xl;
			x[1] = (int) xr;
			x[2] = (int) (x2-(6.0/5.0)*r*Math.cos(alpha));
			int[] y = new int[3];
			y[0] = (int) yl;
			y[1] = (int) yr;
			y[2] = (int) (y2-(6.0/5.0)*r*Math.sin(alpha));

			g2.setColor(Color.gray);
			g2.fillPolygon(x, y, 3);
			
			g2.setColor(Color.black);
			g2.setFont(new Font("Courier New", Font.BOLD, 20));
			g2.drawString(String.valueOf(graph.getEdge(i).getWeight()),
					(graph.getEdge(i).getNode2().getX()+graph.getEdge(i).getNode1().getX()+2*graph.getEdge(i).getNode1().getR())/2, 
					(graph.getEdge(i).getNode2().getY()+graph.getEdge(i).getNode1().getY()+2*graph.getEdge(i).getNode1().getR())/2);
			g2.setFont(new Font("Courier New", Font.PLAIN, 10));
			
		}

		for(int i=0; i<this.selectedNode.length; i++) {
			if(this.selectedNode[i]!=null) {
				Point2D center = new Point2D.Float(selectedNode[i].getX()+25, selectedNode[i].getY()+25);
				float radius = 2.5f*selectedNode[i].getR();
				float[] dist = {0.0f, 1.0f};
				Color[] colors = {Color.BLUE, Color.WHITE};
				if(i == 0)
					colors[0] = Color.BLUE;
				else
					colors[0] = Color.RED;
				RadialGradientPaint p = new RadialGradientPaint(center, radius, dist, colors);
				g2.setPaint(p);
				g2.fillOval(selectedNode[i].getX()-3, selectedNode[i].getY()-3,
						2*selectedNode[i].getR()+5, 2*selectedNode[i].getR()+5);
			}
		}
		
		for(int i=0; i<this.selectedSourceNodes.size(); i++) {
				Point2D center = new Point2D.Float(selectedSourceNodes.get(i).getX()+25, selectedSourceNodes.get(i).getY()+25);
				float radius = 2.5f*selectedSourceNodes.get(i).getR();
				float[] dist = {0.0f, 1.0f};
				Color[] colors = {Color.BLUE, Color.WHITE};
				RadialGradientPaint p = new RadialGradientPaint(center, radius, dist, colors);
				g2.setPaint(p);
				g2.fillOval(selectedSourceNodes.get(i).getX()-3, selectedSourceNodes.get(i).getY()-3,
						2*selectedSourceNodes.get(i).getR()+5, 2*selectedSourceNodes.get(i).getR()+5);
		}

		for(int i=0; i<this.selectedDestinationNodes.size(); i++) {
			Point2D center = new Point2D.Float(selectedDestinationNodes.get(i).getX()+25, selectedDestinationNodes.get(i).getY()+25);
			float radius = 2.5f*selectedDestinationNodes.get(i).getR();
			float[] dist = {0.0f, 1.0f};
			Color[] colors = {Color.RED, Color.WHITE};
			RadialGradientPaint p = new RadialGradientPaint(center, radius, dist, colors);
			g2.setPaint(p);
			g2.fillOval(selectedDestinationNodes.get(i).getX()-3, selectedDestinationNodes.get(i).getY()-3,
					2*selectedDestinationNodes.get(i).getR()+5, 2*selectedDestinationNodes.get(i).getR()+5);
		}
		

		for(int i=0; i<listNodeSize; i++) {
			g2.setColor(graph.getNode(i).getColor());
			g2.fillOval(graph.getNode(i).getX(), graph.getNode(i).getY(),
					2*graph.getNode(i).getR(), 2*graph.getNode(i).getR());
			g2.setColor(Color.black);
			g2.setStroke(new BasicStroke((float) (1.0)));
			g2.drawString(graph.getNode(i).getName(), 
					graph.getNode(i).getX()+10, graph.getNode(i).getY()+graph.getNode(i).getR()+5);
			g2.setColor(Color.black);
			g2.drawOval(graph.getNode(i).getX(), graph.getNode(i).getY(),
					2*graph.getNode(i).getR(), 2*graph.getNode(i).getR());
			/*if(graph.getNode(i).getX()<0) {
			 TODO: need to tell if there is sth outside the graph
				g.drawLine(this.getWidth()-20, this.getHeight()-20, this.getWidth()-25, this.getHeight()-20);
			}*/
		}

	}

	@Override
	public void mouseDragged(MouseEvent e) {
		//TODO need to check to not go outside the panel!!
		if(repaint==false) {
			int x=e.getX();
			int y=e.getY();
			if(repaintAll==false) {
				for(int i=0; i<this.listNodeSize; i++) {
					if(x>=(graph.getNode(i).getX()) && x<=(graph.getNode(i).getX()+2*graph.getNode(i).getR()) 
							&& y>=(graph.getNode(i).getY()) && y<=(graph.getNode(i).getY()+2*graph.getNode(i).getR())) { 
						repaint=true;
						nodeDragged=graph.getNode(i);
					}
				}
			}
			if(repaint==false) {
				if(repaintAll==false) {
					Toolkit tk = Toolkit.getDefaultToolkit();
					setCursor(tk.createCustomCursor(tk.getImage("images/closed.png"),new Point(0,0),"Grabbing Hand"));
					repaintAll=true;
				}
				for(int i=0; i<this.listNodeSize; i++)
					graph.getNode(i).setPosition(graph.getNode(i).getX()+(x-mouseX), graph.getNode(i).getY()+(y-mouseY));
				mouseX=x;
				mouseY=y;
				repaint();
			}
		} else {
			this.nodeDragged.setPosition(e.getX()-25, e.getY()-25);
			repaint();
		}
	}

	//solution to the problem about the speed of the mouse when we drag
	@Override
	public void mouseReleased(MouseEvent e) {
		this.repaint=false;
		this.repaintAll=false;
		setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
		nodeDragged=null;
	}

	@Override
	public void mousePressed(MouseEvent e) {
		mouseX=e.getX();
		mouseY=e.getY();
		if(e.isShiftDown()) {
			this.selectedNode[0] = null;
			this.selectedNode[1] = null;
			if(SwingUtilities.isLeftMouseButton(e)) {
				for(int i=0; i<this.listNodeSize; i++) {
					if(graph.getNode(i).getX()<=mouseX && graph.getNode(i).getX()+2*graph.getNode(i).getR()>=mouseX
							&& graph.getNode(i).getY()<=mouseY && graph.getNode(i).getY()+2*graph.getNode(i).getR()>=mouseY) {
						int index = this.selectedDestinationNodes.indexOf(this.graph.getNode(i));
						if(index != -1)
							this.selectedDestinationNodes.remove(index);
						index = this.selectedSourceNodes.indexOf(this.graph.getNode(i));
						if (index != -1)
							this.selectedSourceNodes.remove(index);
						else
							this.selectedSourceNodes.add(this.graph.getNode(i));
					}
				}
				repaint();
			}
			if(SwingUtilities.isRightMouseButton(e)) {
				for(int i=0; i<this.listNodeSize; i++) {
					if(graph.getNode(i).getX()<=mouseX && graph.getNode(i).getX()+2*graph.getNode(i).getR()>=mouseX
							&& graph.getNode(i).getY()<=mouseY && graph.getNode(i).getY()+2*graph.getNode(i).getR()>=mouseY) {
						int index = this.selectedSourceNodes.indexOf(this.graph.getNode(i));
						if(index != -1)
							this.selectedSourceNodes.remove(index);
						index = this.selectedDestinationNodes.indexOf(this.graph.getNode(i));
						if (index != -1)
							this.selectedDestinationNodes.remove(index);
						else
							this.selectedDestinationNodes.add(this.graph.getNode(i));
					}
				}
				repaint();
			}
		}
		else {
			if(SwingUtilities.isLeftMouseButton(e)) {
				for(int i=0; i<this.listNodeSize; i++) {
					if(graph.getNode(i).getX()<=mouseX && graph.getNode(i).getX()+2*graph.getNode(i).getR()>=mouseX
							&& graph.getNode(i).getY()<=mouseY && graph.getNode(i).getY()+2*graph.getNode(i).getR()>=mouseY) {
						/*If the node is already selected, then unselect it and put the possibly other node selected in slot 1*/
						if(this.selectedNode[0] != null && this.selectedNode[0].equals(graph.getNode(i))) {
							selectedNode[0] = selectedNode[1];
							selectedNode[1] = null;
						}
						else if(this.selectedNode[1] != null && this.selectedNode[1].equals(graph.getNode(i)))
							selectedNode[1] = null;
						/*If there is no node in slot 0 it means that there is no node in slot 1, put the node in slot 0*/
						else if(this.selectedNode[0] == null)
							this.selectedNode[0] = graph.getNode(i);
						/*If there is a not in slot 0, then put the node in slot 1*/
						else
							this.selectedNode[1] = graph.getNode(i);
					}
				}
				repaint();
			}

		if(SwingUtilities.isRightMouseButton(e)) {

			//disables by default some menuItem
			for(int j=0; j<menuItemList.size(); j++) {
				if(menuItemList.get(j).getText().equals("Color")
						|| menuItemList.get(j).getText().equals("bias neuron")
						|| menuItemList.get(j).getText().equals("yes")
						|| menuItemList.get(j).getText().equals("no")
						|| menuItemList.get(j).getText().equals("set weight"))
					menuItemList.get(j).setEnabled(false);
			}

			//Test (loop) if we need to disable or enable the different menuItem in the PopUpMenu
			boolean cont=true;
			for(int i=0; i<this.listNodeSize && cont==true; i++) {
				if(graph.getNode(i).getX()<=mouseX && graph.getNode(i).getX()+2*graph.getNode(i).getR()>=mouseX
						&& graph.getNode(i).getY()<=mouseY && graph.getNode(i).getY()+2*graph.getNode(i).getR()>=mouseY) {
					for(int j=0; j<menuItemList.size(); j++) {
						if(menuItemList.get(j).getText().equals("Color")
								&& graph.getNode(i).getType() != INode.Type.BIAS)
							menuItemList.get(j).setEnabled(true);
						if(menuItemList.get(j).getText().equals("bias neuron")
								&& this.selectedNode[0] != null
								&& this.selectedNode[0].equals(graph.getNode(i))
								&& this.selectedNode[1] == null)
							menuItemList.get(j).setEnabled(true);
						if(graph.getNode(i).getType() == INode.Type.BIAS && menuItemList.get(j).getText().equals("no"))
							menuItemList.get(j).setEnabled(true);
						if(graph.getNode(i).getType() != INode.Type.BIAS && menuItemList.get(j).getText().equals("yes"))
							menuItemList.get(j).setEnabled(true);
					}
					cont=false;
				}
			}
			for(int j=0; j<menuItemList.size(); j++) {
				if(menuItemList.get(j).getText().equals("set weight") 
						&& this.selectedNode[0] != null
						&& this.selectedNode[1] != null)
					for(IEdge ee : this.graph.getListEdges())
						if(ee.contains(this.selectedNode[0]) && ee.contains(this.selectedNode[1]))
							menuItemList.get(j).setEnabled(true);
			}
			//show popUpMenu
			popUpMenu.show(e.getComponent(), mouseX, mouseY);
		}
	}
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().equals("add")) {
			addNode();
		} else if(e.getActionCommand().equals("set weight")) {
			JFrame frame = new JFrame();
			String s = (String)JOptionPane.showInputDialog(
			                    frame,
			                    "Chose the weight:\n",
			                    "Weight setter",
			                    JOptionPane.PLAIN_MESSAGE,
			                    null,
			                    null,
			                    "1.0");
			for(int j=0; j<menuItemList.size(); j++) {
				for(IEdge ee : this.graph.getListEdges())
					if(ee.contains(this.selectedNode[0]) && ee.contains(this.selectedNode[1]))
						ee.setWeight(Double.parseDouble(s));
			}
			
		} else if(e.getActionCommand().equals("yes")) {
			if(this.selectedNode[0]!=null) {
				this.selectedNode[0].setColor(Color.black);
				this.selectedNode[0].setType(INode.Type.BIAS);
			}
		} else if(e.getActionCommand().equals("no")) {
			if(this.selectedNode[0]!=null) {
				this.selectedNode[0].setColor(Color.orange);
				this.selectedNode[0].setType(INode.Type.DEFAULT);
			}
		} else if(e.getActionCommand().equals("remove")) {
			removeNode();
		} else if(e.getActionCommand().equals("link")) {
			if(this.selectedNode[0]!=null && this.selectedNode[1]!=null) {
				IEdge edge = new Edge(this.selectedNode[0], this.selectedNode[1]);
				if(this.graph.contains(edge)==-1)
					addEdge(edge);
				else
					removeEdge(edge);
			}
			else {
				//link all source nodes and destination nodes together
				for(int i = 0 ; i<this.selectedSourceNodes.size(); i++) {
					for(int j = 0 ; j<this.selectedDestinationNodes.size() ; j++) {
						IEdge edge = new Edge(this.selectedSourceNodes.get(i), this.selectedDestinationNodes.get(j));
						if(this.graph.contains(edge)==-1)
							addEdge(edge);
					}
				}
			}
			//clear the lists
			this.selectedSourceNodes.clear();
			this.selectedDestinationNodes.clear();
		} else if(e.getActionCommand().equals("export png")) {
			FileDialog fd = new FileDialog(new JFrame(), "Save", FileDialog.SAVE);
			fd.setFilenameFilter(new FilenameFilter() {
				public boolean accept(File dir, String name) {
					return name.endsWith(".png");
				}
			});
			fd.setFile("graph.png");
			fd.setVisible(true);
			if(fd.getFile().endsWith(".png"))
				export("png",fd.getDirectory()+fd.getFile());
			else
				export("png",fd.getDirectory()+fd.getFile().substring(0, fd.getFile().lastIndexOf("."))+".png");
		} else {
			for(int i=0; i<colorTable.length; i++) {
				if(e.getActionCommand().equals(colorTable[i])) {
					Color color=Color.orange; //default color

					try { //reflection
						Field field = Class.forName("java.awt.Color").getField(colorTable[i]);
						color = (Color) field.get(null);
					} catch (SecurityException e1) {
						e1.printStackTrace();
					} catch (NoSuchFieldException e1) {
						e1.printStackTrace();
					} catch (ClassNotFoundException e1) {
						e1.printStackTrace();
					} catch (IllegalArgumentException e1) {
						e1.printStackTrace();
					} catch (IllegalAccessException e1) {
						e1.printStackTrace();
					}

					//specific color for each node
					for(int j=0; j<this.listNodeSize; j++) {
						if(graph.getNode(j).getX()<=mouseX && graph.getNode(j).getX()+2*graph.getNode(j).getR()>=mouseX
								&& graph.getNode(j).getY()<=mouseY && graph.getNode(j).getY()+2*graph.getNode(j).getR()>=mouseY) {
							graph.getNode(j).setColor(color);
						}
					}
				}
			}
		}
		repaint();
	}

	public void addNode() {
		INode node = new Node(mouseX,mouseY, "nb"+number);
		this.graph.addNode(node);
		this.listNodeSize=this.graph.getListNodesSize();
		number++;
	}
	
	public void addNodeFromFile(Node n) {
		this.graph.addNode(n);
		this.listNodeSize=this.graph.getListNodesSize();
		number++;
	}
	

	public void removeNode() {
		int x=this.mouseX, y=this.mouseY;
		boolean cont=true;
		for(int i=0; i<this.listNodeSize && cont==true; i++) {
			if(x>=(graph.getNode(i).getX()) && x<=(graph.getNode(i).getX()+2*graph.getNode(i).getR())
					&& y>=(graph.getNode(i).getY()) && y<=(graph.getNode(i).getY()+2*graph.getNode(i).getR())) { 
				this.graph.removeEdge(this.graph.getNode(i));
				for(int j=0; j<this.selectedNode.length; j++) {
					if(this.graph.getNode(i).equals(this.selectedNode[j]))
						this.selectedNode[j]=null;
				}
				this.graph.removeNode(i);
				cont=false;
			}
		}
		this.listNodeSize=this.graph.getListNodesSize();
		this.listEdgeSize=this.graph.getListEdgesSize();
	}

	public void addEdge(IEdge edge) {
		edge.setHistoricalNumber(historicalGlobalNumber);
		historicalGlobalNumber++;
		this.graph.addEdge(edge);
		this.listEdgeSize=this.graph.getListEdgesSize();
	}

	public void removeEdge(IEdge edge) {
		this.graph.removeEdge(edge);
		this.listEdgeSize=this.graph.getListEdgesSize();
	}

	public void closeSocialGraph() {
		this.setVisible(false);
	}

	// http://stackoverflow.com/questions/5655908/export-jpanel-graphics-to-png-or-gif-or-jpg
	public void export(String format, String filePath) {
		BufferedImage bi = new BufferedImage(this.getSize().width, this.getSize().height, BufferedImage.TYPE_INT_ARGB); 
		Graphics g = bi.createGraphics();
		this.paint(g);
		g.dispose();
		try {
			ImageIO.write(bi,format,new File(filePath));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public void zoomIn() {
		// for each node, change the radius
		// for each node, change their position (get further away)
	}
	
	public void zoomOut() {
		// for each node, change the radius
		// for each node, change their position (get closer)
	}

	@Override
	public void mouseMoved(MouseEvent e) {}

	@Override
	public void mouseClicked(MouseEvent e) {}

	@Override
	public void mouseEntered(MouseEvent e) {}

	@Override
	public void mouseExited(MouseEvent e) {}

	/*TODO To avoid those empty methods (mouseExited, mouseEntered,...), 
	 * it's better to implement our own listener that extends MouseInputAdapter/MouseAdapter:
	 * http://docs.oracle.com/javase/tutorial/uiswing/events/mousemotionlistener.html 
	 */
	
	public void resetView () {
		this.selectedNode[0] = null;
		this.selectedNode[1] = null;
		repaint();
	}
	
	public void resetMembers() {
		this.number = 0;
		this.historicalGlobalNumber = 1;
	}
}
