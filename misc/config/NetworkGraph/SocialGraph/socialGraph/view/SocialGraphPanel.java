package be.ac.ulb.infof307.g01.socialGraph.view;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Cursor;
import java.awt.FileDialog;
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
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.SwingUtilities;

import be.ac.ulb.infof307.g01.controller.IController;
import be.ac.ulb.infof307.g01.socialGraph.model.Edge;
import be.ac.ulb.infof307.g01.socialGraph.model.Graph;
import be.ac.ulb.infof307.g01.socialGraph.model.IEdge;
import be.ac.ulb.infof307.g01.socialGraph.model.IGraph;
import be.ac.ulb.infof307.g01.socialGraph.model.INode;
import be.ac.ulb.infof307.g01.socialGraph.model.Node;


@SuppressWarnings("serial")
public class SocialGraphPanel extends JPanel implements ISocialGraphPanel, MouseListener, MouseMotionListener, ActionListener {

	private IGraph graph=null;
	private int listNodeSize = 0;
	private int listEdgeSize = 0;

	private INode nodeDragged=null;
	private INode selectedNode[] = new INode[2];

	private boolean repaint=false;
	private boolean repaintAll=false;
	private int mouseX=0;
	private int mouseY=0;
	private String[] colorTable={"red","blue","magenta","orange","gray","pink","green"};
	private JPopupMenu popUpMenu=null;
	private ArrayList<JMenuItem> menuItemList = new ArrayList<JMenuItem>();
	private int number=0;
	private boolean firstTime=true;
	private IController c;

	public SocialGraphPanel(IGraph g, IController c) {
		createJPopUpMenu();
		this.addMouseMotionListener(this);
		this.addMouseListener(this);
		setGraph(g);
		this.c = c;
	}

	public SocialGraphPanel(IController c) {
		this(null, c);
	}

	private void setGraph(IGraph g) {
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
		JMenu menu = new JMenu("Color");
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
			g2.setStroke(new BasicStroke((float) (1.0+graph.getEdge(i).getWeight()/10.0)));
			g2.draw(new Line2D.Double(graph.getEdge(i).getNode1().getX()+graph.getEdge(i).getNode1().getR(),
					graph.getEdge(i).getNode1().getY()+graph.getEdge(i).getNode1().getR(),
					graph.getEdge(i).getNode2().getX()+graph.getEdge(i).getNode2().getR(), 
					graph.getEdge(i).getNode2().getY()+graph.getEdge(i).getNode2().getR()));
			
			g2.drawString(String.valueOf(graph.getEdge(i).getWeight()),
					(graph.getEdge(i).getNode2().getX()+graph.getEdge(i).getNode1().getX()+2*graph.getEdge(i).getNode1().getR())/2, 
					(graph.getEdge(i).getNode2().getY()+graph.getEdge(i).getNode1().getY()+2*graph.getEdge(i).getNode1().getR())/2);
		}

		for(int i=0; i<this.selectedNode.length; i++) {
			if(this.selectedNode[i]!=null) {
				Point2D center = new Point2D.Float(selectedNode[i].getX()+25, selectedNode[i].getY()+25);
				float radius = 2.5f*selectedNode[i].getR();
				float[] dist = {0.0f, 1.0f};
				Color[] colors = {Color.YELLOW, Color.WHITE};
				RadialGradientPaint p =
						new RadialGradientPaint(center, radius, dist, colors);
				g2.setPaint(p);
				g2.fillOval(selectedNode[i].getX()-3, selectedNode[i].getY()-3,
						2*selectedNode[i].getR()+5, 2*selectedNode[i].getR()+5);
			}
		}

		for(int i=0; i<listNodeSize; i++) {
			g2.setColor(graph.getNode(i).getColor());
			g2.fillOval(graph.getNode(i).getX(), graph.getNode(i).getY(),
					2*graph.getNode(i).getR(), 2*graph.getNode(i).getR());
			g2.setColor(Color.black);
			g2.drawString(graph.getNode(i).getName(), 
					graph.getNode(i).getX()+10, graph.getNode(i).getY()+graph.getNode(i).getR()+5);
			g2.setColor(Color.black);
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
					setCursor(tk.createCustomCursor(tk.getImage("src/be/ac/ulb/infof307/g01/socialGraph/images/closed.png"),new Point(0,0),"Grabbing Hand"));
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
			for(int i=0; i<this.listNodeSize; i++) {
				if(graph.getNode(i).getX()<=mouseX && graph.getNode(i).getX()+2*graph.getNode(i).getR()>=mouseX
						&& graph.getNode(i).getY()<=mouseY && graph.getNode(i).getY()+2*graph.getNode(i).getR()>=mouseY) {
					if(this.selectedNode[0]==null)
						this.selectedNode[0]=graph.getNode(i);
					else if(this.selectedNode[0]==graph.getNode(i))
						this.selectedNode[0]=null;
					else if(this.selectedNode[1]==graph.getNode(i))
						this.selectedNode[1]=null;
					else
						this.selectedNode[1]=graph.getNode(i);
				}
			}
			repaint();
		}

		if(SwingUtilities.isRightMouseButton(e)) {

			//disables by default the "Color" menuItem
			for(int j=0; j<menuItemList.size(); j++) {
				if(menuItemList.get(j).getText().equals("Color")) //enable the "Color" menuItem
					menuItemList.get(j).setEnabled(false);
			}

			//Test (loop) if we need to disable or enable the "Color" menuItem in the PopUpMenu
			boolean cont=true;
			for(int i=0; i<this.listNodeSize && cont==true; i++) {
				if(graph.getNode(i).getX()<=mouseX && graph.getNode(i).getX()+2*graph.getNode(i).getR()>=mouseX
						&& graph.getNode(i).getY()<=mouseY && graph.getNode(i).getY()+2*graph.getNode(i).getR()>=mouseY) {
					for(int j=0; j<menuItemList.size(); j++) {
						if(menuItemList.get(j).getText().equals("Color")) //enable the "Color" menuItem
							menuItemList.get(j).setEnabled(true);
					}
					cont=false;
				}
			}

			//show popUpMenu
			popUpMenu.show(e.getComponent(), mouseX, mouseY);
		}
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().equals("add")) {
			addNode();
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
					Color color=Color.blue; //default color

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
		INode node = new Node(mouseX,mouseY,"user"+number);
		this.graph.addNode(node);
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
		this.graph.addEdge(edge);
		this.listEdgeSize=this.graph.getListEdgesSize();
	}

	public void removeEdge(IEdge edge) {
		this.graph.removeEdge(edge);
		this.listEdgeSize=this.graph.getListEdgesSize();
	}

	@Override
	public void closeSocialGraph() {
		this.setVisible(false);
	}

	@Override
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

	@Override
	public void mouseMoved(MouseEvent e) {}

	@Override
	public void mouseClicked(MouseEvent e) {
		
	}

	@Override
	public void mouseEntered(MouseEvent e) {}

	@Override
	public void mouseExited(MouseEvent e) {}

	/*TODO To avoid those empty methods (mouseExited, mouseEntered,...), 
	 * it's better to implement our own listener that extends MouseInputAdapter/MouseAdapter:
	 * http://docs.oracle.com/javase/tutorial/uiswing/events/mousemotionlistener.html 
	 */
}
