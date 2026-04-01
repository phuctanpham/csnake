# csnake

Hướng dẫn commit code:
1. Tích hợp email uit vào tài khoản github và chuyển thành email chính như hình:
(login github >> click avatar >> setting >> email >> add email >> nhập email của bạn >> click add >> chọn email vừa thêm làm primary)

![GitHub](img/github.png)

2. Chạy lệnhlệnh sau để cấu hình:
   - git config --global user.name "MSSV"
   - git config --global user.email "MSSV@ms.uit.edu.vn"

![GitHub](img/gitconfig.png)

3. tạo branch mới để làm việc:
   - git checkout -b MSSV/tên_branch

![GitHub](img/gitcheckout.png)

4. kiểm tra branch hiện tại đang làm việc:
   - git branch

![GitHub](img/gitbranch.png)

5. Kiểm tra trạng thái file đã thay đổi:
   - git status

![GitHub](img/gitstatus.png)

6. chạy lệnh sau để commit code (nên add từng file một để dễ kiểm soát thay vì chạy "	git add ." hoặc "git add -a" để add tất cả file đã thay đổi):
   - git add tên_file_cần_commit
   - git commit -m "MSSV: Nội dung commit"

![GitHub](img/gitcommit.png)

7. Kiểm tra lại bằng lệnh git status để chắc chắn unstaged files --> staged files đã được commit hết rồi mới đẩy code lên github.

![GitHub](img/gitstatus2.png)

8. đẩy code lên github:
   - git push origin MSSV/tên_branch

![GitHub](img/gitpush.png)
