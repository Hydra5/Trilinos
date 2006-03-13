// @HEADER
// ***********************************************************************
// 
//               Thyra: Trilinos Solver Framework Core
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef THYRA_EPETRA_THYRA_WRAPPERS_HPP
#define THYRA_EPETRA_THYRA_WRAPPERS_HPP

#include "Thyra_EpetraTypes.hpp"
#include "Thyra_MPIVectorSpaceBase.hpp"
#include "Thyra_MPIMultiVectorBase.hpp"

namespace Thyra {

/** \defgroup Thyra_Epetra_Thyra_Wrappers_grp  Collection of functions for wrapping and unwrapping Epetra objects

\ingroup Epetra_Thyra_Op_Vec_adapters_grp

This set of functions provides some general utility code for wrapping Epetra
objects in standard %Thyra MPI subclass implementations and for getting Epetra
views of %Thyra objects.

*/

/** \brief Concrete an <tt>MPIVectorSpaceBase</tt> object given an <tt>Epetra_Map</tt>
 * object.
 *
 * \param  epetra_map  [in] The Epetra map defining the partitioning of elements
 *                     to processors.
 *
 * Preconditions:<ul>
 * <li><tt>epetra_map.get() != NULL</tt>
 * </ul>
 *
 * Postconditions:<ul>
 * <li> <tt>return.get() != NULL</tt>
 * <li> The <tt>RefCountPtr</tt> object <tt>epetra_map</tt> is copied into
 *      the <tt>return</tt> object and therefore a memory of epetra_map is
 *      kept.
 * </ul>
 *
 * This uses an <tt>Epetra_Map</tt> object to initialize a compatible
 * <tt>DefaultMPIVectorSpace</tt> object.
 *
 * The fact that this function only accepts an <tt>Epetra_Map</tt> object
 * means that only maps that have elements of size one can be used to define a
 * vector space.  General <tt>Epetra_BlockMap</tt>s can not be used.  This is
 * not a serious limitation since <tt>Epetra_Operator</tt>'s domain and range
 * maps are of type <tt>Epetra_Map</tt>.
 *
 * This function works properly even if Epetra is not compiled with support
 * for MPI (i.e. <tt>HAVE_MPI</tt> is not defined when compiling and linking).
 * If MPI support is not compiled into Epetra, then the dummy implementation
 * defined in <tt>RTOp_mpi.h</tt> is used instead.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<const MPIVectorSpaceDefaultBase<double> >
create_MPIVectorSpaceBase(
	const Teuchos::RefCountPtr<const Epetra_Map> &epetra_map
	);

/** \brief Create a non-<tt>const</tt> <tt>MPIVectorBase</tt> object from
 * a <tt>const> <tt>Epetra_Vector</tt> object.
 *
 * @param  epetra_v  [in] Smart pointer to the <tt>Epetra_Vector</tt> object to wrap.
 * @param  space     [in] The vector space that is compatible with <tt>epetra_v->Map()</tt>.
 *
 * Precondiitions:<ul>
 * <li><tt>epetra_v.get()!=NULL</tt>
 * <li><tt>space.get()!=NULL</tt>
 * </ul>
 *
 * \return The returned <tt>RefCountPtr</tt> object contains a copy of the
 * input <tt>RefCountPtr<Epetra_Vector></tt> wrapped
 * <tt>Epetra_Vector</tt> object.  It is also stated that
 * <tt>*epetra_v</tt> will only be guaranteed to be modifed after the last
 * <tt>RefCountPtr</tt> to the returned <tt>MPIVectorBase</tt> is
 * destroyed.  In addition, <tt>*return</tt> is only valid as long as
 * one <tt>RefCoutPtr</tt> wrapper object still exits.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<MPIVectorBase<double> >
create_MPIVectorBase(
	const Teuchos::RefCountPtr<Epetra_Vector>                                &epetra_v
	,const Teuchos::RefCountPtr<const MPIVectorSpaceBase<double> >           &space
	);

/** \brief Create an <tt>const</tt> <tt>MPIVectorBase</tt> wrapper object
 * for a <tt>const</tt> <tt>Epetra_Vector</tt> object.
 *
 * @param  epetra_v  [in] Smart pointer to the <tt>Epetra_Vector</tt> object to wrap.
 * @param  space     [in] The vector space that is compatible with <tt>epetra_v->Map()</tt>.
 *
 * Precondiitions:<ul>
 * <li><tt>epetra_v.get()!=NULL</tt>
 * <li><tt>space.get()!=NULL</tt>
 * </ul>
 *
 * \return The returned <tt>RefCountPtr</tt> object contains a copy of the
 * input <tt>RefCountPtr<Epetra_Vector></tt> wrapped
 * <tt>Epetra_Vector</tt> object.  In addition, <tt>*return</tt> is only
 * valid as long as one <tt>RefCoutPtr</tt> wrapper object still exits.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<const MPIVectorBase<double> >
create_MPIVectorBase(
	const Teuchos::RefCountPtr<const Epetra_Vector>                          &epetra_v
	,const Teuchos::RefCountPtr<const MPIVectorSpaceBase<double> >           &space
	);

/** \brief Create a non-<tt>const</tt> <tt>MPIMultiVectorBase</tt> object from
 * a <tt>const> <tt>Epetra_MultiVector</tt> object.
 *
 * @param  epetra_mv  [in] Smart pointer to the <tt>Epetra_MultiVector</tt> object to wrap.
 * @param  range      [in] The vector space that is compatible with <tt>epetra_mv->Map()</tt>.
 * @param  domain     [in] The vector space that is compatible with <tt>epetra_mv.NumVectors</tt>.
 *
 * Precondiitions:<ul>
 * <li><tt>epetra_mv.get()!=NULL</tt>
 * <li><tt>range.get()!=NULL</tt>
 * <li><tt>domain.get()!=NULL</tt>
 * </ul>
 *
 * \return The returned <tt>RefCountPtr</tt> object contains a copy of the
 * input <tt>RefCountPtr<Epetra_MultiVector></tt> wrapped
 * <tt>Epetra_MultiVector</tt> object.  It is also stated that
 * <tt>*epetra_mv</tt> will only be guaranteed to be modifed after the last
 * <tt>RefCountPtr</tt> to the returned <tt>MPIMultiVectorBase</tt> is
 * destroyed.  In addition, <tt>*return</tt> is only valid as long as
 * one <tt>RefCoutPtr</tt> wrapper object still exits.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<MPIMultiVectorBase<double> >
create_MPIMultiVectorBase(
	const Teuchos::RefCountPtr<Epetra_MultiVector>                           &epetra_mv
	,const Teuchos::RefCountPtr<const MPIVectorSpaceBase<double> >           &range
	,const Teuchos::RefCountPtr<const ScalarProdVectorSpaceBase<double> >    &domain
	);

/** \brief Create an <tt>const</tt> <tt>MPIMultiVectorBase</tt> wrapper object
 * for a <tt>const</tt> <tt>Epetra_MultiVector</tt> object.
 *
 * @param  epetra_mv  [in] Smart pointer to the <tt>Epetra_MultiVector</tt> object to wrap.
 * @param  range      [in] The vector space that is compatible with <tt>epetra_mv->Map()</tt>.
 * @param  domain     [in] The vector space that is compatible with <tt>epetra_mv.NumVectors</tt>.
 *
 * Precondiitions:<ul>
 * <li><tt>epetra_mv.get()!=NULL</tt>
 * <li><tt>range.get()!=NULL</tt>
 * <li><tt>domain.get()!=NULL</tt>
 * </ul>
 *
 * \return The returned <tt>RefCountPtr</tt> object contains a copy of the
 * input <tt>RefCountPtr<Epetra_MultiVector></tt> wrapped
 * <tt>Epetra_MultiVector</tt> object.  In addition, <tt>*return</tt> is only
 * valid as long as one <tt>RefCoutPtr</tt> wrapper object still exits.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<const MPIMultiVectorBase<double> >
create_MPIMultiVectorBase(
	const Teuchos::RefCountPtr<const Epetra_MultiVector>                     &epetra_mv
	,const Teuchos::RefCountPtr<const MPIVectorSpaceBase<double> >           &range
	,const Teuchos::RefCountPtr<const ScalarProdVectorSpaceBase<double> >    &domain
	);

/** \brief Get a non-<tt>const</tt> <tt>Epetra_Vector</tt> view from a
 * non-<tt>const</tt> <tt>VectorBase</tt> object if possible.
 *
 * Preconditions:<ul>
 * <li> <tt>v.get()!=NULL</tt>
 * <li> <tt>map</tt> must be compatible with <tt>*v.space()</tt>
 * </ul>
 *
 * If a <tt>Teuchos::RefCountPtr<Epetra_Vector></tt> object is already
 * attached to the node of the smart pointer for <tt>mv</tt> then this is
 * returned directly.  If not, then a view of the data in <tt>*v</tt> is
 * created and returned.  In the latter case the smart pointer <tt>v</tt> is
 * copied and attached to the returned RCP object.  Therefore, a temporary
 * <tt>VectorBase</tt> object can be created in the call to this function and
 * the view in <tt>return</tt> will persist until all of the RCP objects to
 * the returned <tt>Epetra_Vector</tt> object go away.
 *
 * Note: the <tt>v</tt> object is not guaranteed to be modified until the last
 * smart pointer to the returned <tt>Epetra_Vector</tt> object is destroyed.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<Epetra_Vector>
get_Epetra_Vector(
	const Epetra_Map                                    &map
	,const Teuchos::RefCountPtr<VectorBase<double> >    &v
	);

/** \brief Get a <tt>const</tt> <tt>Epetra_Vector</tt> view from a
 * <tt>const</tt> <tt>VectorBase</tt> object if possible.
 *
 * Preconditions:<ul>
 * <li> <tt>v.get()!=NULL</tt>
 * <li> <tt>map</tt> must be compatible with <tt>*v.space()</tt>
 * </ul>
 *
 * If a <tt>Teuchos::RefCountPtr<Epetra_Vector></tt> object is already
 * attached to the node of the smart pointer for <tt>mv</tt> then this is
 * returned directly.  If not, then a view of the data in <tt>*v</tt> is
 * created and returned.  In the latter case the smart pointer <tt>v</tt> is
 * copied and attached to the returned RCP object.  Therefore, a temporary
 * <tt>VectorBase</tt> object can be created in the call to this function and
 * the view in <tt>return</tt> will persist until all of the RCP objects to
 * the returned <tt>Epetra_Vector</tt> object go away.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<const Epetra_Vector>
get_Epetra_Vector(
	const Epetra_Map                                         &map 
	,const Teuchos::RefCountPtr<const VectorBase<double> >   &v
	);

/** \brief Get a non-<tt>const</tt> <tt>Epetra_MultiVector</tt> view from a
 * non-<tt>const</tt> <tt>MultiVectorBase</tt> object if possible.
 *
 * Preconditions:<ul>
 * <li> <tt>mv.get()!=NULL</tt>
 * <li> <tt>map</tt> must be compatible with <tt>*mv.range()</tt>
 * </ul>
 *
 * If a <tt>Teuchos::RefCountPtr<Epetra_MultiVector></tt> object is already
 * attached to the node of the smart pointer for <tt>mv</tt> then this is
 * returned directly.  If not, then a view of the data in <tt>*mv</tt> is
 * created and returned.  In the latter case the smart pointer <tt>mv</tt> is
 * copied and attached to the returned RCP object.  Therefore, a temporary
 * <tt>MultiVectorBase</tt> object can be created in the call to this function
 * and the view in <tt>return</tt> will persist until all of the RCP objects
 * to the returned <tt>Epetra_MultiVector</tt> object go away.
 *
 * Note: the <tt>mv</tt> object is not guaranteed to be modified until
 * the last smart pointer to the returned <tt>Epetra_MultiVector</tt>
 * object is destroyed.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<Epetra_MultiVector>
get_Epetra_MultiVector(
	const Epetra_Map                                         &map
	,const Teuchos::RefCountPtr<MultiVectorBase<double> >    &mv
	);

/** \brief Get a <tt>const</tt> <tt>Epetra_MultiVector</tt> view from a
 * <tt>const</tt> <tt>MultiVectorBase</tt> object if possible.
 *
 * Preconditions:<ul>
 * <li> <tt>mv.get()!=NULL</tt>
 * <li> <tt>map</tt> must be compatible with <tt>*mv.range()</tt>
 * </ul>
 *
 * If a <tt>Teuchos::RefCountPtr<const Epetra_MultiVector></tt> object is
 * already attached to the node of the smart pointer for <tt>mv</tt> then this
 * is returned directly.  If not, then a view of the data in <tt>*mv</tt> is
 * created and returned.  In the latter case the smart pointer <tt>mv</tt> is
 * copied and attached to the returned RCP object.  Therefore, a temporary
 * <tt>MultiVectorBase</tt> object can be created in the call to this function
 * and the view in <tt>return</tt> will persist until all of the RCP objects
 * to the returned <tt>Epetra_MultiVector</tt> object go away.
 *
 * \ingroup Thyra_Epetra_Thyra_Wrappers_grp
 */
Teuchos::RefCountPtr<const Epetra_MultiVector>
get_Epetra_MultiVector(
	const Epetra_Map                                              &map 
	,const Teuchos::RefCountPtr<const MultiVectorBase<double> >   &mv
	);

} // namespace Thyra

#endif // THYRA_EPETRA_THYRA_WRAPPERS_HPP
