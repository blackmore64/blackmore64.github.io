from pymongo import MongoClient
from pymongo.errors import PyMongoError

class AnimalShelter:

    # Implementing CRUD operations for the AAC "animals" collection in MongoDB.

    def __init__(
            self,
            username: str = "aacuser",
            password: str = "animals",
            host: str = "nv-desktop-services.apporto.com",
            port: int = 32327,
            db: str = "AAC",
            collection: str = "animals"
    ):
        
        # Initializing the MongoClient here, and also specifying the database and collection

        uri = f"mongodb://{username}:{password}@{host}:{port}/{db}?authSource={db}"
        self.client = MongoClient(uri)
        self.database = self.client[db]
        self.collection = self.database[collection]

    def create(self, data: dict) -> bool:

        # Inserting a single document into the collection here.

        if not data:
            raise ValueError("Nothing to save, because data parameter is empty.")
        
        try:
            self.collection.insert_one(data)
            return True
        
        except PyMongoError as e:
            print(f"[ERROR] insert_one failed: {e}")
            return False
        
    def read(self, query: dict = None) -> list:

        # Creating a query here, for documents in the collection.

        # param_query = A dictionary specifying the find filter. If "None", will return all documents.
        # Will return a list of matching documents, or an empty list if an error occurs, or there are no matches.

        try:
            cursor = self.collection.find(query or {})
            return list(cursor)
        
        except PyMongoError as e:
            print(f"[ERROR] find failed: {e}")
            return []
        

    def update(self, query : dict, update_values : dict, many : bool = False) -> int:
        
        # Creating an update function here, to modify documents in the collection.
        # Here, if many = True, this will use "update_many"; otherwise, "update_one" will be used.
        # This will also return the number of documents modified.

        if not query or not update_values:
            raise ValueError("Both query and update values must be provided.")
        
        try:
            if many:
                result = self.collection.update_many(query, update_values)

            else:
                result = self.collection.update_one(query, update_values)
                
            return result.modified_count
            
        except PyMongoError as e:

            print(f"[ERROR] Update failed: {e}")

            return 0
        

    def delete (self, query : dict, many : bool = False) -> int:

        # Creating a delete function here, to delete any documents matching "query".
        # Similar to the Update function, if "many = True", it will use "delete_many";
        # Otherwise, it will use "delete_one".
        # Also similar to Update, it will return the number of documents deleted.

        if not query:

            raise ValueError("A query must be provided for deletion.")
        
        try:

            if many:
                result = self.collection.delete_many(query)

            else:
                result = self.collection.delete_one(query)

            return result.deleted_count
            
        except PyMongoError as e:
            print(f"[ERROR] Delete failed: {e}")
            return 0
