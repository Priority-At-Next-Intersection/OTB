/*=========================================================================

  Program:   Monteverdi2
  Language:  C++


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See Copyright.txt for details.

  Monteverdi2 is distributed under the CeCILL licence version 2. See
  Licence_CeCILL_V2-en.txt or
  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt for more details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __mvdStackedLayerModel_h
#define __mvdStackedLayerModel_h

//
// Configuration include.
//// Included at first position before any other ones.
#include "ConfigureMonteverdi2.h"


/*****************************************************************************/
/* INCLUDE SECTION                                                           */

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.
#include <QtCore>

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)

//
// Monteverdi includes (sorted by alphabetic order)
#include "mvdAbstractModel.h"

/*****************************************************************************/
/* PRE-DECLARATION SECTION                                                   */

//
// External classes pre-declaration.
namespace
{
}

namespace mvd
{
//
// Internal classes pre-declaration.
class AbstractLayerModel;

/*****************************************************************************/
/* CLASS DEFINITION SECTION                                                  */

/**
 * \class StackedLayerModel
 *
 * \brief WIP.
 */
class Monteverdi2_EXPORT StackedLayerModel :
    public AbstractModel
{

  /*-[ QOBJECT SECTION ]-----------------------------------------------------*/

  Q_OBJECT;

//
// Private types
private:
  // typedef std::vector< AbstractLayerModel * > LayerModelVector;
  typedef std::map< std::string, AbstractLayerModel * > LayerModelMap;

  // typedef std::pair< AbstractLayerModel *, void * > LayerModelPair;
  // typedef std::map< std::string, LayerDataPair > LayerModelPairMap;

  /*-[ PUBLIC SECTION ]------------------------------------------------------*/

//
// Public types.
public:
  typedef std::vector< LayerModelMap::key_type > KeyVector;

  typedef LayerModelMap::size_type SizeType;
  typedef LayerModelMap::key_type KeyType;
  typedef LayerModelMap::const_iterator ConstIterator;

  static const SizeType NIL_INDEX;

//
// Public methods.
public:

  /** \brief Constructor. */
  StackedLayerModel( QObject* parent =NULL );

  /** \brief Destructor. */
  virtual ~StackedLayerModel();

  inline const AbstractLayerModel * operator[]( SizeType ) const;
  inline AbstractLayerModel * operator[]( SizeType );

  KeyType Add( AbstractLayerModel * );

  inline const AbstractLayerModel * At( SizeType ) const;
  inline AbstractLayerModel * At( SizeType );

  inline ConstIterator Begin() const;

  void Clear();

  inline bool Contains( const KeyType & key ) const;
  bool Contains( const AbstractLayerModel * ) const;

  inline SizeType GetCount() const;

  inline const KeyVector & GetKeys() const;

  inline ConstIterator End() const;

  /*
  inline const AbstractLayerModel * Front() const;
  inline AbstractLayerModel * Front();
  */

  template< typename T >
    T * GetCurrent() const;

  template< typename T >
    T * GetCurrent();

  AbstractLayerModel * GetCurrent() const;
  AbstractLayerModel * GetCurrent();

  inline const KeyType & GetCurrentKey() const;

  AbstractLayerModel * GetReference() const;
  AbstractLayerModel * GetReference();

  inline SizeType GetReferenceIndex() const;

  inline bool IsEmpty() const;

  inline void SetCurrent( SizeType );
  void SetCurrent( const KeyType & );
  void SetCurrent( AbstractLayerModel * );

  inline void SetReference( SizeType );
  void SetReference( const KeyType & );

  /*-[ PUBLIC SLOTS SECTION ]------------------------------------------------*/

//
// Public SLOTS.
public slots:
  inline void DeleteCurrent();
  inline void LowerCurrent();
  inline void RaiseCurrent();
  inline void SelectPrevious();
  inline void SelectNext();

  /*-[ SIGNALS SECTION ]-----------------------------------------------------*/

//
// Signals.
signals:
  void AboutToChangeSelectedLayerModel( const StackedLayerModel::KeyType & );
  void SelectedLayerModelChanged( const StackedLayerModel::KeyType & );

  void ContentAboutToBeChanged();
  void ContentAboutToBeReset();
  void ContentChanged();
  void ContentReset();

  void CurrentAboutToBeChanged( size_t );
  void CurrentChanged( size_t );

  void LayerAdded( size_t index );
  void LayerDeleted( size_t index );

  void OrderAboutToBeChanged();
  void OrderChanged();

  void ReferenceAboutToBeChanged( size_t );
  void ReferenceChanged( size_t );

  /*-[ PROTECTED SECTION ]---------------------------------------------------*/

//
// Protected methods.
protected:

//
// Protected attributes.
protected:

  /*-[ PRIVATE SECTION ]-----------------------------------------------------*/

//
// Private methods.
private:
  static KeyType GenerateKey( AbstractLayerModel * );

  void Delete( SizeType );

  inline SizeType FindKey( const KeyType & ) const;

  inline const KeyType & GetKey( SizeType ) const;

  inline SizeType Next( SizeType );
  inline SizeType Prev( SizeType );

  void RaiseLayer( SizeType );
  void LowerLayer( SizeType );

  void RotateLayerUp( SizeType );
  void RotateLayerDown( SizeType );

  // inline void Swap( SizeType, SizeType );

//
// Private attributes.
private:
  static const KeyType NIL_KEY;

  static SizeType m_LayerCount;

  LayerModelMap m_LayerModels;
  KeyVector m_Keys;
  SizeType m_Current;
  SizeType m_Reference;

  /*-[ PRIVATE SLOTS SECTION ]-----------------------------------------------*/

//
// Slots.
private slots:
};

#if _DEBUG
inline void trace( const std::vector< std::string > & );
#endif

} // end namespace 'mvd'.

/*****************************************************************************/
/* INLINE SECTION                                                            */

//
// Qt includes (sorted by alphabetic order)
//// Must be included before system/custom includes.

//
// System includes (sorted by alphabetic order)

//
// ITK includes (sorted by alphabetic order)

//
// OTB includes (sorted by alphabetic order)

//
// Monteverdi includes (sorted by alphabetic order)

namespace mvd
{

/*****************************************************************************/
inline
const AbstractLayerModel *
StackedLayerModel
::operator[]( SizeType i ) const
{
  return At( i );
}

/*****************************************************************************/
inline
AbstractLayerModel *
StackedLayerModel
::operator[]( SizeType i )
{
  return At( i );
}

/*****************************************************************************/
inline
const AbstractLayerModel *
StackedLayerModel
::At( SizeType i ) const
{
  return const_cast< StackedLayerModel * >( this )->At( i );
}

/*****************************************************************************/
inline
AbstractLayerModel *
StackedLayerModel
::At( SizeType i )
{
  assert( !GetKey( i ).empty() );

  LayerModelMap::const_iterator it(
    m_LayerModels.find( GetKey( i ) )
  );

  assert( it!=m_LayerModels.end() );

  return it->second;
}

/*****************************************************************************/
inline
StackedLayerModel::ConstIterator
StackedLayerModel
::Begin() const
{
  return m_LayerModels.begin();
}

/*****************************************************************************/
inline
bool
StackedLayerModel
::Contains( const KeyType & key ) const
{
  return m_LayerModels.find( key )!=m_LayerModels.end();
}

/*****************************************************************************/
inline
void
StackedLayerModel
::DeleteCurrent()
{
  // qDebug() << this << "::DeleteCurrent()";

  Delete( m_Current );
}

/*****************************************************************************/
inline
StackedLayerModel::SizeType
StackedLayerModel
::FindKey( const KeyType & key ) const
{
  KeyVector::const_iterator it( std::find( m_Keys.begin(), m_Keys.end(), key ) );

  if( it==m_Keys.end() )
    return StackedLayerModel::NIL_INDEX;

  return std::distance( m_Keys.begin(), it );
}

/*****************************************************************************/
inline
StackedLayerModel::SizeType
StackedLayerModel
::GetCount() const
{
  return m_LayerModels.size();
}

/*****************************************************************************/
inline
const StackedLayerModel::KeyVector &
StackedLayerModel
::GetKeys() const
{
  return m_Keys;
}

/*****************************************************************************/
inline
StackedLayerModel::ConstIterator
StackedLayerModel
::End() const
{
  return m_LayerModels.end();
}

/*****************************************************************************/
template< typename T >
inline
T *
StackedLayerModel
::GetCurrent()
{
  return dynamic_cast< T * >( GetCurrent() );
}

/*****************************************************************************/
template< typename T >
inline
T *
StackedLayerModel
::GetCurrent() const
{
  return dynamic_cast< T * >( GetCurrent() );
}

/*****************************************************************************/
inline
AbstractLayerModel *
StackedLayerModel
::GetCurrent()
{
  if( m_Current>=GetCount() )
    return NULL;

  LayerModelMap::const_iterator it(
    m_LayerModels.find( m_Keys[ m_Current ] )
  );

  if( it==m_LayerModels.end() )
    return NULL;

  return it->second;
}

/*****************************************************************************/
inline
AbstractLayerModel *
StackedLayerModel
::GetCurrent() const
{
  return const_cast< StackedLayerModel * >( this )->GetCurrent();
}

/*****************************************************************************/
inline
const StackedLayerModel::KeyType &
StackedLayerModel
::GetCurrentKey() const
{
  return GetKey( m_Current );
}

/*****************************************************************************/
inline
const StackedLayerModel::KeyType &
StackedLayerModel
::GetKey( SizeType index ) const
{
  if( index>=GetCount() )
    return StackedLayerModel::NIL_KEY;

  return m_Keys[ index ];
}

/*****************************************************************************/
inline
AbstractLayerModel *
StackedLayerModel
::GetReference()
{
  if( m_Reference>=GetCount() )
    return NULL;

  LayerModelMap::const_iterator it(
    m_LayerModels.find( m_Keys[ m_Reference ] )
  );

  if( it==m_LayerModels.end() )
    return NULL;

  return it->second;
}

/*****************************************************************************/
inline
AbstractLayerModel *
StackedLayerModel
::GetReference() const
{
  return const_cast< StackedLayerModel * >( this )->GetReference();
}

/*****************************************************************************/
inline
StackedLayerModel::SizeType
StackedLayerModel
::GetReferenceIndex() const
{
  return m_Reference;
}

/*****************************************************************************/
inline
bool
StackedLayerModel
::IsEmpty() const
{
  return m_LayerModels.empty();
}

/*****************************************************************************/
inline
void
StackedLayerModel
::LowerCurrent()
{
  if( GetCount()<2 )
    return;

  assert( m_Current<GetCount() );

  if( m_Current==GetCount()-1 )
    RotateLayerDown( 1 );
  else
    LowerLayer( m_Current );
}

/*****************************************************************************/
inline
StackedLayerModel::SizeType
StackedLayerModel
::Next( SizeType index )
{
  return
    index>=GetCount()
    ? 0
    : ( index + 1 ) % GetCount();
}

/*****************************************************************************/
inline
StackedLayerModel::SizeType
StackedLayerModel
::Prev( SizeType index )
{
  return
    index>=GetCount() || index==0
    ? GetCount() - 1
    : index - 1;
}

/*****************************************************************************/
inline
void
StackedLayerModel
::RaiseCurrent()
{
  if( GetCount()<2 )
    return;

  assert( m_Current<GetCount() );

  if( m_Current==0 )
    RotateLayerUp( 1 );
  else
    RaiseLayer( m_Current );
}

/*****************************************************************************/
inline
void
StackedLayerModel
::SelectNext()
{
  if( IsEmpty() )
    return;

  SetCurrent( Next( m_Current ) );
}

/*****************************************************************************/
inline
void
StackedLayerModel
::SelectPrevious()
{
  if( IsEmpty() )
    return;

  SetCurrent( Prev( m_Current ) );
}

/*****************************************************************************/
inline
void
StackedLayerModel
::SetCurrent( SizeType index )
{
  // qDebug() << this << "::SetCurrent(" << index << ")";

  if( index==m_Current )
    return;

  KeyType key( GetKey( index  ) );

  // qDebug()
  //   << QString( "'%1'" ).arg( GetCurrentKey().c_str() )
  //   << "->"
  //   << QString( "'%1'" ).arg( key.c_str() );

  emit CurrentAboutToBeChanged( index );
  emit AboutToChangeSelectedLayerModel( key );

  m_Current = index;

  emit CurrentChanged( index );
  emit SelectedLayerModelChanged( key );
}

/*****************************************************************************/
inline
void
StackedLayerModel
::SetReference( SizeType index )
{
  // qDebug() << this << "::SetReference(" << index << ")";

  if( index==m_Reference )
    return;

  emit ReferenceAboutToBeChanged( index );

  m_Current = index;

  emit ReferenceChanged( index );
}

/*****************************************************************************/
#if _DEBUG

inline
void
trace( const std::vector< std::string > & strv )
{
  typedef std::vector< std::string > StringVector;

  qDebug() << "{";

  for( StringVector::const_iterator it( strv.begin() );
       it!=strv.end();
       ++it )
    qDebug() << QString( "%1" ).arg( it->c_str() );

  qDebug() << "}";
}

#endif

} // end namespace 'mvd'

#endif // __mvdStackedLayerModel_h