import java.awt.Color;
import java.awt.Dimension;

import java.awt.FileDialog;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

import javax.swing.ImageIcon;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.event.MenuEvent;
import javax.swing.event.MenuListener;

/**
 * NNFrame (=Neural Network Frame) Class.
 * @author Brian Delhaisse.
 */
@SuppressWarnings("serial")
public class NNFrame extends JFrame implements ActionListener, MouseListener, MenuListener {
	
	private GraphPanel panel;
	private int XCurrentNode;
	
	public NNFrame(GraphPanel p) {
		super();
		this.panel = p;
		XCurrentNode = 0;
		this.setTitle("Artificial Neural Network for NEAT");
		this.setMinimumSize(new Dimension(400,300));
		
		//Create the menu bar.
		JMenuBar menuBar = new JMenuBar();
		JMenu menu = new JMenu("File");
		JMenuItem menuItem = new JMenuItem("Open");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		menuItem = new JMenuItem("Save");
		menuItem.addActionListener(this);
		menu.add(menuItem);
		menuBar.add(menu);
		menu = new JMenu();
		menu.setIcon(new ImageIcon("images/zoomin.png"));
		menu.setToolTipText("Zoom In");
		//menu.setActionCommand("Zoom In");
		menu.addMenuListener(this);
		menu.addMouseListener(this);
		menuBar.add(menu);
		menu = new JMenu();
		menu.setIcon(new ImageIcon("images/zoomout.png"));
		menu.setToolTipText("Zoom Out");
		menu.addMenuListener(this);
		menu.addMouseListener(this);
		menuBar.add(menu);
		this.setJMenuBar(menuBar);
		
		//Create the graph panel.
		this.add(panel);
		
		this.setVisible(true);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getActionCommand().equals("Save")) {
			FileDialog fd = new FileDialog(new JFrame(), "Save", FileDialog.SAVE);
			fd.setFilenameFilter(new FilenameFilter() {
				public boolean accept(File dir, String name) {
					return name.endsWith(".txt");
				}
			});
			fd.setFile("NN.txt");
			fd.setVisible(true);
			makeGenomeFile(fd.getFile());
			}
		else if (e.getActionCommand().equals("Open")) {
			FileDialog fd = new FileDialog(new JFrame(), "Open", FileDialog.LOAD);
			fd.setVisible(true);
			this.makeGraph(fd.getFile());
		}
	}
	@Override
	public void mouseClicked(MouseEvent arg0) {
		//System.out.println("mouseClicked");
	}

	@Override
	public void mouseEntered(MouseEvent arg0) {
		//System.out.println("mouseEntered");
	}

	@Override
	public void mouseExited(MouseEvent arg0) {
		//System.out.println("mouseExited");
	}

	@Override
	public void mousePressed(MouseEvent arg0) {
		//System.out.println("mousePressed");
	}

	@Override
	public void mouseReleased(MouseEvent arg0) {
		//System.out.println("mouseReleased");
	}

	@Override
	public void menuCanceled(MenuEvent e) {
		//System.out.println("MenuCanceled");
	}

	@Override
	public void menuDeselected(MenuEvent e) {
		//System.out.println("MenuDeselected");
	}

	@Override
	public void menuSelected(MenuEvent e) {
		//System.out.println("MenuSelected");
	}

	public void makeGenomeFile(String filePath) {
		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream(new File(filePath));
			
			fos.write(this.panel.getGraph().writeHeader("1").getBytes());
			fos.write(this.panel.getGraph().writeTrait().getBytes());
			fos.write(this.panel.getGraph().writeNodes().getBytes());
			fos.write(this.panel.getGraph().writeGenes().getBytes());
			fos.write(this.panel.getGraph().writeEndLine("1").getBytes());
		} catch(IOException e) {
			e.printStackTrace();
		} finally{
			try {
				if(fos != null)
					fos.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	public void makeGraph(String filePath) {
		BufferedReader buf = null;
		FileInputStream fis = null;
		try {
			this.clearGraph();
			fis = new FileInputStream(new File(filePath));
			buf = new BufferedReader(new InputStreamReader(fis));
			String line;
			line = buf.readLine();
			while((line = buf.readLine())!=null) {
				String beginning = line.substring(0,line.indexOf(" "));
				if(beginning.equals("node"))
					this.addNode(line);
				if(beginning.equals("gene"))
					this.addEdge(line);
			}
			this.moveNode();
			this.panel.repaint();
		} catch(IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if(fis != null)
					fis.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	public void clearGraph() {
		//TODO : properly clean the graph instead of creating a new one
		this.panel.resetView();
		this.panel.resetMembers();
		this.panel.setGraph(new Graph());
	}
	
	public ArrayList<String> lineToArrayList(String line, int numberOfElements) {
		ArrayList<String> aL = new ArrayList<String>();
		//StringBuilder endOfLine = new StringBuilder(line);
		int cursor = 0;
		for(int i = 0 ; i<numberOfElements ; i++) {
			int tmp = line.indexOf(" ",cursor);
			if(tmp == -1)
				aL.add(line.substring(cursor));
			else
				aL.add(line.substring(cursor,tmp));
			cursor = tmp+1;
		}
		return aL;
	}
	
	public void addNode(String line) {
		ArrayList<String> aL = lineToArrayList(line, 5);

		String id = aL.get(1);
		/*In the genome files, the first node has number 1, in the implementation, it has id nb0 :(*/
		int identifier = Integer.parseInt(id)-1;
		Node nodeToAdd = new Node("nb"+identifier);
		this.panel.addNodeFromFile(nodeToAdd);
		String type = aL.get(4);
		if(type.equals("0"))
			nodeToAdd.setType(Node.Type.HIDDEN);
		else if(type.equals("1"))
			nodeToAdd.setType(Node.Type.INPUT);
		else if(type.equals("2"))
			nodeToAdd.setType(Node.Type.OUTPUT);
		else if(type.equals("3")) {
			nodeToAdd.setType(Node.Type.BIAS);
			nodeToAdd.setColor(Color.BLACK);
		}
		else
			System.out.println("This node has no type, there might an error in the genome file");
	}
	
	public void addEdge(String line) {
		ArrayList<String> aL = lineToArrayList(line,9);
		IEdge e = new Edge(this.panel.getGraph().getNodeFromId(""+(Integer.parseInt(aL.get(2))-1)),
				this.panel.getGraph().getNodeFromId(""+(Integer.parseInt(aL.get(3))-1)),
						Double.parseDouble(aL.get(4)),
								aL.get(6));
		this.panel.addEdge(e);
	}
	
	public void moveNode() {
		int XInit = 50;
		int YInit = 50;
		int XspaceBetweenNodes = 350;
		int YspaceBetweenNodes = 15;
		int maxNumberOfHiddenNodesOnOneColumn = 20;
		
		int XInputOrBias = XInit;
		int YInputOrBias = YInit;
		int XHidden = XInit + XspaceBetweenNodes;
		int YHidden = YInit;
		int XOutput = XHidden + XspaceBetweenNodes;
		int YOutput = YInit;
		int countHiddenNodes = 0;
		
		for(INode n : this.panel.getGraph().getListNodes()) {
			if(n.getType().equals(Node.Type.INPUT) || n.getType().equals(Node.Type.BIAS)) {
				n.setPosition(XInputOrBias,YInputOrBias);
				YInputOrBias += (2*n.getR()+YspaceBetweenNodes);
			}
			else if(n.getType().equals(Node.Type.HIDDEN)) {
				n.setPosition(XHidden,YHidden);
				countHiddenNodes ++;
				countHiddenNodes = countHiddenNodes % maxNumberOfHiddenNodesOnOneColumn;
				if(countHiddenNodes == maxNumberOfHiddenNodesOnOneColumn) {
					XHidden += (2*n.getR()+XspaceBetweenNodes);
					YHidden = YInit;
				}
				else
					YHidden += (2*n.getR()+YspaceBetweenNodes);
			}
			else if (n.getType().equals(Node.Type.OUTPUT)) {
				n.setPosition(XOutput,YOutput);
				YOutput += (2*n.getR()+YspaceBetweenNodes);
			}
			else
				System.out.println("This node has no type defines, there might be a problem in the genome file");
		}
	}
}

