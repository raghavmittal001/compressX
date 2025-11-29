import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import os

root = tk.Tk()
root.title("Huffman File Compressor / Decompressor")
root.geometry("1000x600")
root.resizable(False, False)

selected_file = tk.StringVar()
output_file = tk.StringVar()

def browse_file():
    filename = filedialog.askopenfilename()
    selected_file.set(filename)


def compress_action():
    if not selected_file.get():
        return messagebox.showerror("Error", "Select an input file first!")

    output = filedialog.asksaveasfilename(defaultextension=".huff",
                                          title="Save compressed file as")
    if not output:
        return
    

    try:
        subprocess.run(["./huffman", "1", selected_file.get(), output], check=True)
        messagebox.showinfo("Success", f"Compression Successful!\nSaved as: {output}")
    except Exception as e:
        messagebox.showerror("Error", "Compression failed!\n" + str(e))


def decompress_action():
    if not selected_file.get():
        return messagebox.showerror("Error", "Select a .huff file first!")

    output = filedialog.asksaveasfilename(title="Save decompressed file as")
    if not output:
        return
    
    try:
        subprocess.run(["./huffman", "2", selected_file.get(), output], check=True)
        messagebox.showinfo("Success", f"Decompression Successful!\nSaved as: {output}")
    except Exception as e:
        messagebox.showerror("Error", "Decompression failed!\n" + str(e))

tk.Label(root, text="Huffman Compression Tool", font=("Arial", 34, "bold")).pack(pady=20)

tk.Entry(root, textvariable=selected_file, width=90).pack(pady=20)
tk.Button(root, text="Select File", command=browse_file, width=24,bg="black",fg="white").pack()

tk.Button(root, text="Compress", command=compress_action, width=40, bg="green", fg="white").pack(pady=40)
tk.Button(root, text="Decompress", command=decompress_action, width=40, bg="blue", fg="white").pack()


root.mainloop()
