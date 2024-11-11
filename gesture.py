import cv2
import mediapipe as mp

mp_hands = mp.solutions.hands
hands = mp_hands.Hands()

cap = cv2.VideoCapture(0)

circle_color = (255, 105, 180) 
line_color = (50, 50, 50)     
line_thickness = 4 

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)

    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_frame)

    if results.multi_hand_landmarks:
        for landmarks in results.multi_hand_landmarks:

            index_finger = landmarks.landmark[8]
            thumb = landmarks.landmark[4]
            wrist = landmarks.landmark[0]

            index_x, index_y = int(index_finger.x * frame.shape[1]), int(index_finger.y * frame.shape[0])
            thumb_x, thumb_y = int(thumb.x * frame.shape[1]), int(thumb.y * frame.shape[0])
            wrist_x, wrist_y = int(wrist.x * frame.shape[1]), int(wrist.y * frame.shape[0])

            with open("hand_coordinates.txt", 'w') as file:
                file.write(f"{index_x},{index_y}\n")
                file.write(f"{thumb_x},{thumb_y}\n")


            cv2.line(frame, (index_x, index_y), (thumb_x, thumb_y), line_color, line_thickness)

            cv2.circle(frame, (index_x, index_y), 10, circle_color, -1)  

            cv2.circle(frame, (thumb_x, thumb_y), 10, circle_color, -1)  

            cv2.circle(frame, (wrist_x, wrist_y), 5, (100, 100, 100), -1) 

            

    cv2.imshow("Hand Gesture Detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
