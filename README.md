# Fortnite-External
### Note
- Mapper and Driver not included.
### Features
- Mouse movement via writing delta angles
- Optimized ESP with Snaplines
- Working Softaim with visibility-detection & knocked-detection
### How to use
- Open the solution and make sure dependencies are included correctly
- import your driver ioctl communication into ```src/lib/*``` and resolve according methods in ```src/memory/memory.hpp```
- make sure your driver communication utilizes and externs the ```src/assembly.asm``` object to avoid detection via MASM
- build as release x64 and run when ur in the lobby
### Showcase
<img width="1920" height="1080" alt="{816CABE6-DA20-4273-8E13-B1E3C1EB76C9}" src="https://github.com/user-attachments/assets/179a1587-30af-46ae-a7cc-74aa75296823" />
<img width="1920" height="1080" alt="{3AF8B659-541A-43DC-B6ED-58A641E76EB6}" src="https://github.com/user-attachments/assets/2030b81e-c8da-4ef7-97cd-bfc1989af2b0" />
<img width="1920" height="1080" alt="{0E550562-6FE6-4285-874B-8A1325885694}" src="https://github.com/user-attachments/assets/41dd6fc7-af0f-4da2-83f9-391fbffbf3ea" />
