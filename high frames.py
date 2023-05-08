import cv2

#cap = cv2.VideoCapture('http://166.130.18.45:1024/mjpg/video.mjpg')
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

human_cascade = cv2.CascadeClassifier('cascades/haarcascade_frontalface_default.xml')


while(True):
    # Read a frame from the camera.
    ret, frame = cap.read()

    # Our operations on the frame come here.
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    humans = human_cascade.detectMultiScale(gray, 1.9, 1)
    
    # Detect faces and draw boxes around them.
    for (x,y,w,h) in humans:
         cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
         
    cv2.imshow("Face Recognition", frame)
    
    # Wait for a key press to stop process.
    key = cv2.waitKey(1)
    if key == ord('q') or key == ord('Q'):
        sys.exit()

# Release the camera and close the window.
cap.release()
cv2.destroyAllWindows()
