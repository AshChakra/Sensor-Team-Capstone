import torch
import cv2
import time

object_detect = torch.hub.load("ultralytics/yolov5", "yolov5m", pretrained=True)
object_detect.to(torch.device('cuda:0'))

cap = cv2.VideoCapture(0)

print("writing to pipe\n");
while True:

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    ret, frame = cap.read()
    frame = frame[:, :frame.shape[1]//2, :]

    found_objects = object_detect(cv2.flip(frame,0))
    
    fd = open("cam_data.txt", "w")
    
    for index, row in found_objects.pandas().xyxy[0].reset_index().iterrows():
        if row["name"] == "person" and row["confidence"] > 0.1:
            fd.write("1\n")
            print("1")
        else:
            fd.write("0\n")
            print("0")

    fd.close()
    time.sleep(0.5)
    #cv2.imshow("feed", frame)