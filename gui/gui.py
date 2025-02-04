import time  # Biblioteca pentru manipularea timpului
import serial  # Biblioteca pentru comunicarea serială cu un dispozitiv extern
import matplotlib.pyplot as plt  # Biblioteca pentru crearea graficelor
from matplotlib.animation import FuncAnimation  # Modul pentru animarea graficului in timp real
from matplotlib.widgets import Button  # Modul pentru adaugarea butoanelor

class LEDMonitor:
    def __init__(self, port='COM5', baudrate=9600):
        # Initializeaza comunicarea seriala 
        self.ser = serial.Serial(port, baudrate, timeout=0.1)
        self.start_time = time.time()  # Timpul initial pentru referinta in grafic
        self.time_data = [0]  # Lista ce stocheaza momentele de timp
        self.time_data_last = 0  # Ultimul timp in care s-a schimbat starea LED-ului
        self.led_states = [0]  # Lista ce stocheaza starile LED-ului (0 = stins, 1 = aprins)
        self.current_led_duration = 0  # Durata curenta pentru care LED-ul ramane aprins
        self.last_led_on_duration = 0  # Ultima durata pentru care LED-ul a fost aprins
        self.is_led_on = False  # Variabila ce indica daca LED-ul este aprins
        self.last_update_time = time.time()  # Ultimul moment de actualizare a graficului

        # figura si axele graficului
        self.fig, self.ax = plt.subplots(figsize=(12, 6))
        plt.subplots_adjust(bottom=0.4, top=0.85)  
        self.fig.canvas.manager.set_window_title("Monitorizare LED")  

        #  butonul de resetare 
        button_ax = plt.axes([0.75, 0.05, 0.2, 0.1]) 
        self.reset_button = Button(button_ax, 'Resetare', color='#2E86C1', hovercolor='#1B4F72')
        self.reset_button.label.set_fontsize(14)
        self.reset_button.label.set_fontweight('bold')
        self.reset_button.label.set_color('white')
        button_ax.set_facecolor('#2E86C1') 
        button_ax.spines['top'].set_visible(False) 
        button_ax.spines['right'].set_visible(False)  
        button_ax.spines['left'].set_visible(False) 
        button_ax.spines['bottom'].set_visible(False)  
        self.reset_button.on_clicked(self.reset_graph)  

        self.last_duration_text = plt.axes([0.05, 0.25, 0.9, 0.05])
        self.last_duration_label = self.last_duration_text.text(
            0.5, 0.5, "LED-ul a fost aprins pentru 0 secunde",
            transform=self.last_duration_text.transAxes,
            fontsize=14, fontweight='bold',
            color="darkblue", ha='center')
        self.last_duration_text.axis('off')

       
        self.legend_text = plt.axes([0.05, 0.15, 0.9, 0.05])
        self.legend_label = self.legend_text.text(
            0.5, 0.5, "Legenda: LED Aprins (albastru), LED Stins (gri)",
            transform=self.legend_text.transAxes,
            fontsize=14, fontweight='bold',
            color="black", ha='center')
        self.legend_text.axis('off')

    #functie pentru citirea datelor de pe portul serial
    def read_serial_data(self):
        try:
            if self.ser.in_waiting > 0:  # Verifica daca exista date disponibile
                data = self.ser.readline().decode().strip()  # Citeste si proceseaza linia de date
                if data.isdigit():  # Verifica daca este un numar valid
                    self.current_led_duration = int(data)  # Converteste durata in int
                    print(f"LED aprins pentru {self.current_led_duration} secunde")
                    self.time_data.append(time.time() - self.start_time)  # Salveaza timpul curent
                    self.time_data_last = time.time() - self.start_time  # Actualizeaza ultimul timp de aprindere
                    self.led_states.append(1)  # Adauga starea LED aprins
                    self.is_led_on = True  # Marcheaza LED-ul ca aprins
        except Exception as e:
            print(f"Serial read error: {e}")

    #functie pentru actualizarea graficului
    def update_graph(self, frame):
        current_time = time.time() - self.start_time
        self.read_serial_data()  # Citeste datele de la portul serial

        # Verifica daca LED-ul trebuie stins
        if self.is_led_on and current_time - self.time_data_last > self.current_led_duration:
            self.time_data.append(current_time)
            self.led_states.append(0)  # Adauga starea LED stins
            self.last_led_on_duration = self.current_led_duration  # Salveaza ultima durata aprinsa
            self.current_led_duration = 0  # Reseteaza durata curenta
            self.time_data_last = 0
            self.is_led_on = False
        else:
            self.time_data.append(current_time)
            self.led_states.append(self.led_states[-1])  # Pastreaza aceeasi stare

        plt.style.use('ggplot')  # Aplica stil vizual pentru grafic
        self.ax.cla()
        self.ax.set_title("Monitorizare LED", fontsize=18, fontweight='bold')
        self.ax.set_xlabel("Timp (secunde)", fontsize=14, fontweight='bold')
        self.ax.set_ylabel("Stare LED", fontsize=14, fontweight='bold')
        self.ax.set_yticks([0, 1])
        self.ax.set_yticklabels(["Stins", "Aprins"], fontsize=12)
        self.ax.grid(axis="y", linestyle="--", alpha=0.5)

        # Deseneaza liniile pe grafic
        for i in range(1, len(self.time_data)):
            start = self.time_data[i - 1]
            end = self.time_data[i]
            color = 'blue' if self.led_states[i - 1] == 1 else 'grey'
            self.ax.hlines(self.led_states[i - 1], start, end, colors=color, linewidth=6)

            # Adauga linii verticale la tranzitii
            if self.led_states[i] != self.led_states[i - 1]:
                self.ax.vlines(end, 0, 1, colors='black', linestyles='dashed', alpha=0.7)

        # Actualizeaza textul pentru ultima durata LED aprins
        if self.last_led_on_duration > 0:
            self.last_duration_label.set_text(f"LED-ul a fost aprins pentru {self.last_led_on_duration:.2f} secunde")
        else:
            self.last_duration_label.set_text("LED-ul a fost aprins pentru 0 secunde")

    #functie pentru resetarea graficului, fara inchiderea aplicatiei
    def reset_graph(self, event):
        print("Resetare grafic...")
        self.start_time = time.time()
        self.time_data = [0]
        self.led_states = [0]
        self.current_led_duration = 0
        self.last_led_on_duration = 0
        self.is_led_on = False
        self.time_data_last = 0
        self.last_update_time = time.time()
        self.ax.cla()
        self.last_duration_label.set_text("LED-ul a fost aprins pentru 0 secunde")

    #functie pentru inchiderea gui si a conexiunii seriale
    def close(self, event):
        print("Inchidere aplicatie...")
        self.ser.close()
        plt.close('all')
        exit(0)

    def run(self):
        self.fig.canvas.mpl_connect('close_event', self.close)
        ani = FuncAnimation(self.fig, self.update_graph, interval=50, cache_frame_data=False)
        plt.show()

# Inițializare și rulare
monitor = LEDMonitor()
monitor.run()
