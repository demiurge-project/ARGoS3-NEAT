/* ############################################################################
 * 
 * TestKeyListener.java : démonstration de la capture d'événements issus du
 *                        clavier.
 * 
 * Auteur : Christophe Jacquet, Supélec
 * 
 * Historique
 * 2006-12-19  Création
 * 
 * ############################################################################
 */
 
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
 
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextArea;
import javax.swing.SwingUtilities;
 
 
/**
 * Programme de test des KeyListener : affiche une fenêtre où un JLabel
 * affiche les codes de touches appuyées et relâchées.
 */
public class TestKeyListener extends JFrame implements KeyListener {
    //private static final long serialVersionUID = -5222658361778310082L;
	private StringBuilder weight;
	private JTextArea textArea;
	private boolean finished;
	
    public TestKeyListener() {
        // création de la fenêtre
        super("TestKeyListener");
        
        /*Initializers*/
        weight = new StringBuilder("");
        finished = false;

        // ajout d'un seul composant dans cette fenêtre : un JLabel
        textArea = new JTextArea();
         
        // ajoute un écouteur d'événements personnalisé à la fenêtre
        //addKeyListener(new TitreKeyListener(label));
         
        addKeyListener(this);
        
        // réglage des dimensions de la fenêtre
        setPreferredSize(new Dimension(300, 100));
        pack();
    }
        
        public void keyReleased(KeyEvent e) {
        	System.out.println(e.getKeyCode());
        	if(e.getKeyCode() == 10)
        		setFinished(true);
        	else {
        		weight.append(e.getKeyChar());
        		this.textArea.setText(getWeight());
        	}
        }
        
        public void keyPressed(KeyEvent e) {
        }
     
        public void keyTyped(KeyEvent e) {
        }
        
        public String getWeight() {
        	return weight.toString();
        }
        
        public boolean getFinished() {
        	return finished;
        }
        public void setFinished(boolean finished_) {
        	finished = finished_;
        }
        
}
