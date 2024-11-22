
import os
import json
from github import Github
from pymongo import MongoClient
from dotenv import load_dotenv
import datetime

# Load environment variables
load_dotenv()
GITHUB_TOKEN = os.getenv('GITHUB_TOKEN')
MONGODB_URI = os.getenv('MONGODB_URI')

# Define excluded paths and files
EXCLUDED_PATHS = {
    '.github/workflows',
    '.github'
}

EXCLUDED_FILES = {
    'store_repo_structure.py',
    'store_repo_structure.yml',
    'store_repo_structure_extended.yml'
}

def should_exclude(path, name):
    """
    Check if the path or file should be excluded
    
    Args:
        path (str): The path to check
        name (str): The file name to check
        
    Returns:
        bool: True if should be excluded, False otherwise
    """
    # Check if path is in excluded paths
    if any(path.startswith(excluded) for excluded in EXCLUDED_PATHS):
        return True
        
    # Check if file name is in excluded files
    if name in EXCLUDED_FILES:
        return True
        
    return False

def fetch_repo_structure(repo_name, owner):
    """
    Fetch repository structure excluding workflow files
    
    Args:
        repo_name (str): Name of the repository
        owner (str): Owner of the repository
        
    Returns:
        dict: Repository structure
    """
    g = Github(GITHUB_TOKEN)
    repo = g.get_repo(f'{owner}/{repo_name}')
    return process_folder(repo)

def process_folder(repo, path=''):
    """
    Process repository folder and create structure
    
    Args:
        repo: GitHub repository object
        path (str): Current path being processed
        
    Returns:
        dict: Folder structure
    """
    try:
        contents = repo.get_contents(path)
        folder_structure = {
            'readme': None,
            'media': [],
            'assets': []
        }
        
        for item in contents:
            # Skip excluded paths and files
            if should_exclude(item.path, item.name):
                continue
                
            if item.type == 'file':
                raw_url = item.download_url
                if item.name.lower() == 'readme.md':
                    folder_structure['readme'] = raw_url
                elif item.name.endswith(('.mp4', '.jpg', '.jpeg', '.png', '.gif')):
                    folder_structure['media'].append({
                       # 'name': item.name,
                        'url': raw_url,
                        # 'path': item.path
                    })
                else:
                    folder_structure['assets'].append({
                        # 'name': item.name,
                        'url': raw_url,
                        # 'path': item.path
                    })
            elif item.type == 'dir':
                # Process subdirectory only if not excluded
                if not should_exclude(item.path, item.name):
                    subfolder = process_folder(repo, item.path)
                    if any(subfolder.values()):  # Only add non-empty folders
                        folder_structure[item.name] = subfolder
        
        # Clean up empty lists and None values
        return {k: v for k, v in folder_structure.items() if v and (isinstance(v, dict) or len(v) > 0)}
        
    except Exception as e:
        print(f"Error processing path {path}: {str(e)}")
        return {}

def store_in_mongodb(repo_name, structure):
    """
    Store repository structure in MongoDB
    
    Args:
        repo_name (str): Name of the repository
        structure (dict): Repository structure to store
    """
    try:
        client = MongoClient(MONGODB_URI)
        db = client['myDatabase']
        collection = db['repoStructures']
        
        # Add metadata to the structure
        document = {
            'repo': repo_name,
            'structure': structure,
            'lastUpdated': datetime.datetime.utcnow(),
            'totalFiles': count_files(structure)
        }
        
        collection.update_one(
            {'repo': repo_name},
            {'$set': document},
            upsert=True
        )
        print(f'Repository structure for {repo_name} stored successfully')
        
    except Exception as e:
        print(f"Error storing in MongoDB: {str(e)}")
    finally:
        client.close()

def count_files(structure):
    """
    Count total files in the structure
    
    Args:
        structure (dict): Repository structure
        
    Returns:
        int: Total number of files
    """
    total = 0
    if isinstance(structure, dict):
        for key, value in structure.items():
            if key == 'media':
                total += len(value)
            elif key == 'assets':
                total += len(value)
            elif key == 'readme' and value is not None:
                total += 1
            elif isinstance(value, dict):
                total += count_files(value)
    return total

def main():
    """
    Main function to execute the repository structure storage
    """
    try:
        repo_full_name = os.getenv('GITHUB_REPOSITORY')
        if not repo_full_name:
            raise ValueError("GITHUB_REPOSITORY environment variable not set")
            
        owner, repo_name = repo_full_name.split('/')
        structure = fetch_repo_structure(repo_name, owner)
        store_in_mongodb(repo_name, structure)
        
    except Exception as e:
        print(f"Error in main execution: {str(e)}")

if __name__ == '__main__':
    main()

