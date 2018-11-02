# JarKom-Milestone-1
milestone 1 jaringan komputer

*CARA MENJALANKAN SOFTWARE*
Pastika Sistem Operasi yang digunakan adalah windows dan sudah terinstall MinGW.
Untuk melakukan kompilasi jalankan command "make" atau "MinGW32-make" pada command prompt yang terintegrasi dengan MinGW
Program terbagi menjadi dua bagian yaitu sender dan receiver.
Dari kedua program tersebut, yang perlu dijalankan pertama kali adalah receiver. hal ini dilakukan dengan cara menjalankan command ".\sendfile <filename> <windowsize> <buffersize> <destination_ip> <destination_port>".
Setelah itu, akan muncul tulisan waiting for data di command prompt jika berhasil dan ada tulisan error jika tidak.
Lalu, untuk mengirim file, jalankan command ".\recvfile <filename> <windowsize> <buffersize> <destination_port>".
Setelah itu, akan terlihat proses pengiriman file dengan packet dan pengiriman ack.
setelah proses selesai, isi dari outputfile harus sama dengan input file.

**fungsi-fungsi**
createPacket(data,sequenceNumber): 
Mereturn packet dengan isi data dari packet tsb adalah data dan sequence numbernya adalah sequenceNumber.
packetToString(p,message):
mengonversi packet menjadi string yang disimpan ke message yang nantinya akan dikirim ke receiver.
parsePacket(packet):
Mereturn konversi string yang dikirimkan oleh sender menjadi struktur data packet
createACK(ack):
membuat ack dengan struktur data ack, nextSequenceNumber, dan checksum
checksum(x,length):
menghitung

**Pembagian Tugas**
Krishna Aurelio N : Sender
M Ilyas Mustafa : Packet
Dion Saputra : Receiver