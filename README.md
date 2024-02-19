# PCOM Web Client

### Implementation:
- Started with laboratory 9 HTML skeleton for request composition files.
- Used recommended JSON library for managing objects.
### Defined functions:
`extract_cookie`: extract received cookie.
`extract_token`: extract received token.
`create_user`: create JSON for user info.
`create_book`: create JSON for book info.
`notification_client`: notify client based on input.
### Main function reads user commands and executes corresponding actions:
`REGISTER`: POST request to register user.
`LOGIN`: POST request to login.
`ENTER_LIBRARY`: GET request with login cookie to enter library.
`GET_BOOKS`: GET request with token to retrieve books.
`GET_BOOK`: GET request with token and book ID to retrieve specific book.
`ADD_BOOK`: POST request with token to add a book.
`DELETE_BOOK`: DELETE request with token and book ID to delete a book.
`LOGOUT`: GET request to logout and invalidate cookie/token.
`EXIT`: Program termination.
- Used enum to control user states: not logged in, logged in, in library.
