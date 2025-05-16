import ctypes
import tkinter as tk
from tkinter import messagebox, scrolledtext

# Load the shared C++ library
lib = ctypes.CDLL('./libspamdetector.so')

lib.check_spam.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.check_spam.restype = ctypes.c_char_p

lib.learn_feedback.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_int]
lib.learn_feedback.restype = None

def check_spam(sender, message):
    sender_b = sender.encode('utf-8')
    message_b = message.encode('utf-8')
    result = lib.check_spam(sender_b, message_b)
    return result.decode('utf-8')

def learn_feedback(sender, message, is_spam):
    sender_b = sender.encode('utf-8')
    message_b = message.encode('utf-8')
    lib.learn_feedback(sender_b, message_b, int(is_spam))

# --- Tkinter GUI ---

class SpamDetectorApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Spam Detector GUI")
        self.geometry("600x500")

        # Sender Label and Entry
        tk.Label(self, text="Sender Email:").pack(anchor="w", padx=10, pady=(10, 0))
        self.sender_entry = tk.Entry(self, width=70)
        self.sender_entry.pack(padx=10, pady=5)

        # Message Label and Textbox
        tk.Label(self, text="Message:").pack(anchor="w", padx=10, pady=(10, 0))
        self.message_text = scrolledtext.ScrolledText(self, width=70, height=15)
        self.message_text.pack(padx=10, pady=5)

        # Check Button
        self.check_btn = tk.Button(self, text="Check Spam", command=self.check_spam_action)
        self.check_btn.pack(pady=10)

        # Result Label
        self.result_var = tk.StringVar()
        self.result_label = tk.Label(self, textvariable=self.result_var, font=("Arial", 14))
        self.result_label.pack(pady=5)

        # Feedback Buttons (appear after checking)
        self.feedback_frame = tk.Frame(self)
        self.spam_btn = tk.Button(self.feedback_frame, text="Mark as Spam", command=lambda: self.give_feedback(True), state="disabled")
        self.not_spam_btn = tk.Button(self.feedback_frame, text="Mark as Not Spam", command=lambda: self.give_feedback(False), state="disabled")
        self.spam_btn.pack(side="left", padx=10)
        self.not_spam_btn.pack(side="left", padx=10)
        self.feedback_frame.pack(pady=10)

        self.last_sender = None
        self.last_message = None

    def check_spam_action(self):
        sender = self.sender_entry.get().strip()
        message = self.message_text.get("1.0", tk.END).strip()

        if not sender or not message:
            messagebox.showwarning("Input Error", "Please enter both sender email and message.")
            return

        result = check_spam(sender, message)
        self.result_var.set(f"Result: {result}")

        # Enable feedback buttons
        self.spam_btn.config(state="normal")
        self.not_spam_btn.config(state="normal")

        # Save last inputs for feedback
        self.last_sender = sender
        self.last_message = message

    def give_feedback(self, is_spam):
        if self.last_sender is None or self.last_message is None:
            messagebox.showerror("Error", "No message checked yet.")
            return

        learn_feedback(self.last_sender, self.last_message, is_spam)

        feedback_str = "marked as Spam" if is_spam else "marked as Not Spam"
        messagebox.showinfo("Feedback Received", f"Message {feedback_str} and learned.")

        # Disable feedback buttons until next check
        self.spam_btn.config(state="disabled")
        self.not_spam_btn.config(state="disabled")

if __name__ == "__main__":
    app = SpamDetectorApp()
    app.mainloop()

